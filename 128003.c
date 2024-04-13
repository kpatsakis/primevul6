
static int change_mtu(struct net_device *dev, unsigned long new_mtu)
{
	return dev_set_mtu(dev, (int)new_mtu);