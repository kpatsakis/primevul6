
static const void *net_namespace(struct device *d)
{
	struct net_device *dev = to_net_dev(d);

	return dev_net(dev);