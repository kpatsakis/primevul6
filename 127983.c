
static int change_flags(struct net_device *dev, unsigned long new_flags)
{
	return dev_change_flags(dev, (unsigned int)new_flags, NULL);