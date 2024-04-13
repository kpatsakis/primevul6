int do_coredump(long signr, int exit_code, struct pt_regs * regs)
{
	char corename[CORENAME_MAX_SIZE + 1];
	struct mm_struct *mm = current->mm;
	struct linux_binfmt * binfmt;
	struct inode * inode;
	struct file * file;
	int retval = 0;
	int fsuid = current->fsuid;
	int flag = 0;
	int ispipe = 0;

	audit_core_dumps(signr);

	binfmt = current->binfmt;
	if (!binfmt || !binfmt->core_dump)
		goto fail;
	down_write(&mm->mmap_sem);
	if (!mm->dumpable) {
		up_write(&mm->mmap_sem);
		goto fail;
	}

	/*
	 *	We cannot trust fsuid as being the "true" uid of the
	 *	process nor do we know its entire history. We only know it
	 *	was tainted so we dump it as root in mode 2.
	 */
	if (mm->dumpable == 2) {	/* Setuid core dump mode */
		flag = O_EXCL;		/* Stop rewrite attacks */
		current->fsuid = 0;	/* Dump root private */
	}
	mm->dumpable = 0;

	retval = coredump_wait(exit_code);
	if (retval < 0)
		goto fail;

	/*
	 * Clear any false indication of pending signals that might
	 * be seen by the filesystem code called to write the core file.
	 */
	clear_thread_flag(TIF_SIGPENDING);

	if (current->signal->rlim[RLIMIT_CORE].rlim_cur < binfmt->min_coredump)
		goto fail_unlock;

	/*
	 * lock_kernel() because format_corename() is controlled by sysctl, which
	 * uses lock_kernel()
	 */
 	lock_kernel();
	ispipe = format_corename(corename, core_pattern, signr);
	unlock_kernel();
 	if (ispipe) {
		/* SIGPIPE can happen, but it's just never processed */
 		if(call_usermodehelper_pipe(corename+1, NULL, NULL, &file)) {
 			printk(KERN_INFO "Core dump to %s pipe failed\n",
			       corename);
 			goto fail_unlock;
 		}
 	} else
 		file = filp_open(corename,
				 O_CREAT | 2 | O_NOFOLLOW | O_LARGEFILE | flag,
				 0600);
	if (IS_ERR(file))
		goto fail_unlock;
	inode = file->f_path.dentry->d_inode;
	if (inode->i_nlink > 1)
		goto close_fail;	/* multiple links - don't dump */
	if (!ispipe && d_unhashed(file->f_path.dentry))
		goto close_fail;

	/* AK: actually i see no reason to not allow this for named pipes etc.,
	   but keep the previous behaviour for now. */
	if (!ispipe && !S_ISREG(inode->i_mode))
		goto close_fail;
	if (!file->f_op)
		goto close_fail;
	if (!file->f_op->write)
		goto close_fail;
	if (!ispipe && do_truncate(file->f_path.dentry, 0, 0, file) != 0)
		goto close_fail;

	retval = binfmt->core_dump(signr, regs, file);

	if (retval)
		current->signal->group_exit_code |= 0x80;
close_fail:
	filp_close(file, NULL);
fail_unlock:
	current->fsuid = fsuid;
	complete_all(&mm->core_done);
fail:
	return retval;
}