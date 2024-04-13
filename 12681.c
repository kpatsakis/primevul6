static int smack_inode_copy_up(struct dentry *dentry, struct cred **new)
{

	struct task_smack *tsp;
	struct smack_known *skp;
	struct inode_smack *isp;
	struct cred *new_creds = *new;

	if (new_creds == NULL) {
		new_creds = prepare_creds();
		if (new_creds == NULL)
			return -ENOMEM;
	}

	tsp = smack_cred(new_creds);

	/*
	 * Get label from overlay inode and set it in create_sid
	 */
	isp = smack_inode(d_inode(dentry->d_parent));
	skp = isp->smk_inode;
	tsp->smk_task = skp;
	*new = new_creds;
	return 0;
}