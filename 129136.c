static int load_misc_binary(struct linux_binprm *bprm, struct pt_regs *regs)
{
	Node *fmt;
	struct file * interp_file = NULL;
	char iname[BINPRM_BUF_SIZE];
	char *iname_addr = iname;
	int retval;
	int fd_binary = -1;
	struct files_struct *files = NULL;

	retval = -ENOEXEC;
	if (!enabled)
		goto _ret;

	/* to keep locking time low, we copy the interpreter string */
	read_lock(&entries_lock);
	fmt = check_file(bprm);
	if (fmt)
		strlcpy(iname, fmt->interpreter, BINPRM_BUF_SIZE);
	read_unlock(&entries_lock);
	if (!fmt)
		goto _ret;

	if (!(fmt->flags & MISC_FMT_PRESERVE_ARGV0)) {
		retval = remove_arg_zero(bprm);
		if (retval)
			goto _ret;
	}

	if (fmt->flags & MISC_FMT_OPEN_BINARY) {

		files = current->files;
		retval = unshare_files();
		if (retval < 0)
			goto _ret;
		if (files == current->files) {
			put_files_struct(files);
			files = NULL;
		}
		/* if the binary should be opened on behalf of the
		 * interpreter than keep it open and assign descriptor
		 * to it */
 		fd_binary = get_unused_fd();
 		if (fd_binary < 0) {
 			retval = fd_binary;
 			goto _unshare;
 		}
 		fd_install(fd_binary, bprm->file);

		/* if the binary is not readable than enforce mm->dumpable=0
		   regardless of the interpreter's permissions */
		if (file_permission(bprm->file, MAY_READ))
			bprm->interp_flags |= BINPRM_FLAGS_ENFORCE_NONDUMP;

		allow_write_access(bprm->file);
		bprm->file = NULL;

		/* mark the bprm that fd should be passed to interp */
		bprm->interp_flags |= BINPRM_FLAGS_EXECFD;
		bprm->interp_data = fd_binary;

 	} else {
 		allow_write_access(bprm->file);
 		fput(bprm->file);
 		bprm->file = NULL;
 	}
	/* make argv[1] be the path to the binary */
	retval = copy_strings_kernel (1, &bprm->interp, bprm);
	if (retval < 0)
		goto _error;
	bprm->argc++;

	/* add the interp as argv[0] */
	retval = copy_strings_kernel (1, &iname_addr, bprm);
	if (retval < 0)
		goto _error;
	bprm->argc ++;

	bprm->interp = iname;	/* for binfmt_script */

	interp_file = open_exec (iname);
	retval = PTR_ERR (interp_file);
	if (IS_ERR (interp_file))
		goto _error;

	bprm->file = interp_file;
	if (fmt->flags & MISC_FMT_CREDENTIALS) {
		/*
		 * No need to call prepare_binprm(), it's already been
		 * done.  bprm->buf is stale, update from interp_file.
		 */
		memset(bprm->buf, 0, BINPRM_BUF_SIZE);
		retval = kernel_read(bprm->file, 0, bprm->buf, BINPRM_BUF_SIZE);
	} else
		retval = prepare_binprm (bprm);

	if (retval < 0)
		goto _error;

	retval = search_binary_handler (bprm, regs);
	if (retval < 0)
		goto _error;

	if (files) {
		put_files_struct(files);
		files = NULL;
	}
_ret:
	return retval;
_error:
	if (fd_binary > 0)
		sys_close(fd_binary);
	bprm->interp_flags = 0;
	bprm->interp_data = 0;
_unshare:
	if (files)
		reset_files_struct(current, files);
	goto _ret;
}