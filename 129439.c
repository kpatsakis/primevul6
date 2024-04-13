static int rt6_stats_seq_open(struct inode *inode, struct file *file)
{
	return single_open_net(inode, file, rt6_stats_seq_show);
}