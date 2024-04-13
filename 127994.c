
static void net_get_ownership(struct device *d, kuid_t *uid, kgid_t *gid)
{
	struct net_device *dev = to_net_dev(d);
	const struct net *net = dev_net(dev);

	net_ns_get_ownership(net, uid, gid);