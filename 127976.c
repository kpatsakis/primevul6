
static int change_proto_down(struct net_device *dev, unsigned long proto_down)
{
	return dev_change_proto_down(dev, (bool)proto_down);