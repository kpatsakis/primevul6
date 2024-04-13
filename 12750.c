static int smack_kernel_create_files_as(struct cred *new,
					struct inode *inode)
{
	struct inode_smack *isp = smack_inode(inode);
	struct task_smack *tsp = smack_cred(new);

	tsp->smk_forked = isp->smk_inode;
	tsp->smk_task = tsp->smk_forked;
	return 0;
}