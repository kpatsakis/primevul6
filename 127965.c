
static ssize_t ifalias_store(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t len)
{
	struct net_device *netdev = to_net_dev(dev);
	struct net *net = dev_net(netdev);
	size_t count = len;
	ssize_t ret = 0;

	if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
		return -EPERM;

	/* ignore trailing newline */
	if (len >  0 && buf[len - 1] == '\n')
		--count;

	if (!rtnl_trylock())
		return restart_syscall();

	if (dev_isalive(netdev)) {
		ret = dev_set_alias(netdev, buf, count);
		if (ret < 0)
			goto err;
		ret = len;
		netdev_state_change(netdev);
	}
err:
	rtnl_unlock();

	return ret;