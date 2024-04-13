static int smk_bu_file(struct file *file, int mode, int rc)
{
	struct task_smack *tsp = smack_cred(current_cred());
	struct smack_known *sskp = tsp->smk_task;
	struct inode *inode = file_inode(file);
	struct inode_smack *isp = smack_inode(inode);
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (isp->smk_flags & SMK_INODE_IMPURE)
		pr_info("Smack Unconfined Corruption: inode=(%s %ld) %s\n",
			inode->i_sb->s_id, inode->i_ino, current->comm);

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;

	smk_bu_mode(mode, acc);
	pr_info("Smack %s: (%s %s %s) file=(%s %ld %pD) %s\n", smk_bu_mess[rc],
		sskp->smk_known, smk_of_inode(inode)->smk_known, acc,
		inode->i_sb->s_id, inode->i_ino, file,
		current->comm);
	return 0;
}