static int sp_open_dev(struct net_device *dev)
{
	struct sixpack *sp = netdev_priv(dev);

	if (sp->tty == NULL)
		return -ENODEV;
	return 0;
}