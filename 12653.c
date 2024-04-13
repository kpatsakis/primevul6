static int smack_dentry_create_files_as(struct dentry *dentry, int mode,
					struct qstr *name,
					const struct cred *old,
					struct cred *new)
{
	struct task_smack *otsp = smack_cred(old);
	struct task_smack *ntsp = smack_cred(new);
	struct inode_smack *isp;
	int may;

	/*
	 * Use the process credential unless all of
	 * the transmuting criteria are met
	 */
	ntsp->smk_task = otsp->smk_task;

	/*
	 * the attribute of the containing directory
	 */
	isp = smack_inode(d_inode(dentry->d_parent));

	if (isp->smk_flags & SMK_INODE_TRANSMUTE) {
		rcu_read_lock();
		may = smk_access_entry(otsp->smk_task->smk_known,
				       isp->smk_inode->smk_known,
				       &otsp->smk_task->smk_rules);
		rcu_read_unlock();

		/*
		 * If the directory is transmuting and the rule
		 * providing access is transmuting use the containing
		 * directory label instead of the process label.
		 */
		if (may > 0 && (may & MAY_TRANSMUTE))
			ntsp->smk_task = isp->smk_inode;
	}
	return 0;
}