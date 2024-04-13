static void smack_task_to_inode(struct task_struct *p, struct inode *inode)
{
	struct inode_smack *isp = smack_inode(inode);
	struct smack_known *skp = smk_of_task_struct_obj(p);

	isp->smk_inode = skp;
	isp->smk_flags |= SMK_INODE_INSTANT;
}