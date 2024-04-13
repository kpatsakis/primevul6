static int ipv6_route_open(struct inode *inode, struct file *file)
{
	return single_open_net(inode, file, ipv6_route_show);
}