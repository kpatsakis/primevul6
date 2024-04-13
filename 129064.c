int search_binary_handler(struct linux_binprm *bprm,struct pt_regs *regs)
{
	int try,retval;
	struct linux_binfmt *fmt;
#ifdef __alpha__
	/* handle /sbin/loader.. */
	{
	    struct exec * eh = (struct exec *) bprm->buf;

	    if (!bprm->loader && eh->fh.f_magic == 0x183 &&
		(eh->fh.f_flags & 0x3000) == 0x3000)
	    {
		struct file * file;
		unsigned long loader;

		allow_write_access(bprm->file);
		fput(bprm->file);
		bprm->file = NULL;

		loader = bprm->vma->vm_end - sizeof(void *);

		file = open_exec("/sbin/loader");
		retval = PTR_ERR(file);
		if (IS_ERR(file))
			return retval;

		/* Remember if the application is TASO.  */
		bprm->sh_bang = eh->ah.entry < 0x100000000UL;

		bprm->file = file;
		bprm->loader = loader;
		retval = prepare_binprm(bprm);
		if (retval<0)
			return retval;
		/* should call search_binary_handler recursively here,
		   but it does not matter */
	    }
	}
#endif
	retval = security_bprm_check(bprm);
	if (retval)
		return retval;

	/* kernel module loader fixup */
	/* so we don't try to load run modprobe in kernel space. */
	set_fs(USER_DS);

	retval = audit_bprm(bprm);
	if (retval)
		return retval;

	retval = -ENOENT;
	for (try=0; try<2; try++) {
		read_lock(&binfmt_lock);
		for (fmt = formats ; fmt ; fmt = fmt->next) {
			int (*fn)(struct linux_binprm *, struct pt_regs *) = fmt->load_binary;
			if (!fn)
				continue;
			if (!try_module_get(fmt->module))
				continue;
			read_unlock(&binfmt_lock);
			retval = fn(bprm, regs);
			if (retval >= 0) {
				put_binfmt(fmt);
				allow_write_access(bprm->file);
				if (bprm->file)
					fput(bprm->file);
				bprm->file = NULL;
				current->did_exec = 1;
				proc_exec_connector(current);
				return retval;
			}
			read_lock(&binfmt_lock);
			put_binfmt(fmt);
			if (retval != -ENOEXEC || bprm->mm == NULL)
				break;
			if (!bprm->file) {
				read_unlock(&binfmt_lock);
				return retval;
			}
		}
		read_unlock(&binfmt_lock);
		if (retval != -ENOEXEC || bprm->mm == NULL) {
			break;
#ifdef CONFIG_KMOD
		}else{
#define printable(c) (((c)=='\t') || ((c)=='\n') || (0x20<=(c) && (c)<=0x7e))
			if (printable(bprm->buf[0]) &&
			    printable(bprm->buf[1]) &&
			    printable(bprm->buf[2]) &&
			    printable(bprm->buf[3]))
				break; /* -ENOEXEC */
			request_module("binfmt-%04x", *(unsigned short *)(&bprm->buf[2]));
#endif
		}
	}
	return retval;
}