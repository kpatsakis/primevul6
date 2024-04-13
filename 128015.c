
static int change_group(struct net_device *dev, unsigned long new_group)
{
	dev_set_group(dev, (int)new_group);
	return 0;