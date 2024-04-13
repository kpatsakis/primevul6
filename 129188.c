int compat_do_execve(char * filename,
	compat_uptr_t __user *argv,
	compat_uptr_t __user *envp,
	struct pt_regs * regs)
{
	struct linux_binprm *bprm;
	struct file *file;
	int retval;

	retval = -ENOMEM;
	bprm = kzalloc(sizeof(*bprm), GFP_KERNEL);
	if (!bprm)
		goto out_ret;

	file = open_exec(filename);
	retval = PTR_ERR(file);
	if (IS_ERR(file))
		goto out_kfree;

	sched_exec();

	bprm->file = file;
	bprm->filename = filename;
	bprm->interp = filename;

	retval = bprm_mm_init(bprm);
	if (retval)
		goto out_file;

	bprm->argc = compat_count(argv, MAX_ARG_STRINGS);
	if ((retval = bprm->argc) < 0)
		goto out_mm;

	bprm->envc = compat_count(envp, MAX_ARG_STRINGS);
	if ((retval = bprm->envc) < 0)
		goto out_mm;

	retval = security_bprm_alloc(bprm);
	if (retval)
		goto out;

	retval = prepare_binprm(bprm);
	if (retval < 0)
		goto out;

	retval = copy_strings_kernel(1, &bprm->filename, bprm);
	if (retval < 0)
		goto out;

	bprm->exec = bprm->p;
	retval = compat_copy_strings(bprm->envc, envp, bprm);
	if (retval < 0)
		goto out;

	retval = compat_copy_strings(bprm->argc, argv, bprm);
	if (retval < 0)
		goto out;

	retval = search_binary_handler(bprm, regs);
	if (retval >= 0) {
		/* execve success */
		security_bprm_free(bprm);
		acct_update_integrals(current);
		kfree(bprm);
		return retval;
	}

out:
	if (bprm->security)
		security_bprm_free(bprm);

out_mm:
	if (bprm->mm)
		mmput(bprm->mm);

out_file:
	if (bprm->file) {
		allow_write_access(bprm->file);
		fput(bprm->file);
	}

out_kfree:
	kfree(bprm);

out_ret:
	return retval;
}