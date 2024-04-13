static int smk_bu_inode(struct inode *inode, int mode, int rc)
{
	struct task_smack *tsp = smack_cred(current_cred());
	struct inode_smack *isp = smack_inode(inode);
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (isp->smk_flags & SMK_INODE_IMPURE)
		pr_info("Smack Unconfined Corruption: inode=(%s %ld) %s\n",
			inode->i_sb->s_id, inode->i_ino, current->comm);

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;
	if (rc == SMACK_UNCONFINED_SUBJECT &&
	    (mode & (MAY_WRITE | MAY_APPEND)))
		isp->smk_flags |= SMK_INODE_IMPURE;

	smk_bu_mode(mode, acc);

	pr_info("Smack %s: (%s %s %s) inode=(%s %ld) %s\n", smk_bu_mess[rc],
		tsp->smk_task->smk_known, isp->smk_inode->smk_known, acc,
		inode->i_sb->s_id, inode->i_ino, current->comm);
	return 0;
}