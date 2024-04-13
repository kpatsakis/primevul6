int prepare_binprm(struct linux_binprm *bprm)
{
	int mode;
	struct inode * inode = bprm->file->f_path.dentry->d_inode;
	int retval;

	mode = inode->i_mode;
	if (bprm->file->f_op == NULL)
		return -EACCES;

	bprm->e_uid = current->euid;
	bprm->e_gid = current->egid;

	if(!(bprm->file->f_path.mnt->mnt_flags & MNT_NOSUID)) {
		/* Set-uid? */
		if (mode & S_ISUID) {
			current->personality &= ~PER_CLEAR_ON_SETID;
			bprm->e_uid = inode->i_uid;
		}

		/* Set-gid? */
		/*
		 * If setgid is set but no group execute bit then this
		 * is a candidate for mandatory locking, not a setgid
		 * executable.
		 */
		if ((mode & (S_ISGID | S_IXGRP)) == (S_ISGID | S_IXGRP)) {
			current->personality &= ~PER_CLEAR_ON_SETID;
			bprm->e_gid = inode->i_gid;
		}
	}

	/* fill in binprm security blob */
	retval = security_bprm_set(bprm);
	if (retval)
		return retval;

	memset(bprm->buf,0,BINPRM_BUF_SIZE);
	return kernel_read(bprm->file,0,bprm->buf,BINPRM_BUF_SIZE);
}