static int smack_file_receive(struct file *file)
{
	int rc;
	int may = 0;
	struct smk_audit_info ad;
	struct inode *inode = file_inode(file);
	struct socket *sock;
	struct task_smack *tsp;
	struct socket_smack *ssp;

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, file->f_path);

	if (inode->i_sb->s_magic == SOCKFS_MAGIC) {
		sock = SOCKET_I(inode);
		ssp = sock->sk->sk_security;
		tsp = smack_cred(current_cred());
		/*
		 * If the receiving process can't write to the
		 * passed socket or if the passed socket can't
		 * write to the receiving process don't accept
		 * the passed socket.
		 */
		rc = smk_access(tsp->smk_task, ssp->smk_out, MAY_WRITE, &ad);
		rc = smk_bu_file(file, may, rc);
		if (rc < 0)
			return rc;
		rc = smk_access(ssp->smk_in, tsp->smk_task, MAY_WRITE, &ad);
		rc = smk_bu_file(file, may, rc);
		return rc;
	}
	/*
	 * This code relies on bitmasks.
	 */
	if (file->f_mode & FMODE_READ)
		may = MAY_READ;
	if (file->f_mode & FMODE_WRITE)
		may |= MAY_WRITE;

	rc = smk_curacc(smk_of_inode(inode), may, &ad);
	rc = smk_bu_file(file, may, rc);
	return rc;
}