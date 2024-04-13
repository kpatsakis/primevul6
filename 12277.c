static struct net_device *rose_ax25_dev_find(char *devname)
{
	struct net_device *dev;

	if ((dev = __dev_get_by_name(&init_net, devname)) == NULL)
		return NULL;

	if ((dev->flags & IFF_UP) && dev->type == ARPHRD_AX25)
		return dev;

	return NULL;
}