
static ssize_t phys_port_name_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct net_device *netdev = to_net_dev(dev);
	ssize_t ret = -EINVAL;

	if (!rtnl_trylock())
		return restart_syscall();

	if (dev_isalive(netdev)) {
		char name[IFNAMSIZ];

		ret = dev_get_phys_port_name(netdev, name, sizeof(name));
		if (!ret)
			ret = sprintf(buf, "%s\n", name);
	}
	rtnl_unlock();

	return ret;