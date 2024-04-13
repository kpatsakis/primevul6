
static int change_carrier(struct net_device *dev, unsigned long new_carrier)
{
	if (!netif_running(dev))
		return -EINVAL;
	return dev_change_carrier(dev, (bool)new_carrier);