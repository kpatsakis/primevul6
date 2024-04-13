/* use same locking and permission rules as SIF* ioctl's */
static ssize_t netdev_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t len,
			    int (*set)(struct net_device *, unsigned long))
{
	struct net_device *netdev = to_net_dev(dev);
	struct net *net = dev_net(netdev);
	unsigned long new;
	int ret = -EINVAL;

	if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
		return -EPERM;

	ret = kstrtoul(buf, 0, &new);
	if (ret)
		goto err;

	if (!rtnl_trylock())
		return restart_syscall();

	if (dev_isalive(netdev)) {
		ret = (*set)(netdev, new);
		if (ret == 0)
			ret = len;
	}
	rtnl_unlock();
 err:
	return ret;