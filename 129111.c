static void bm_clear_inode(struct inode *inode)
{
	kfree(inode->i_private);
}