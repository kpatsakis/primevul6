
static ssize_t phys_switch_id_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct net_device *netdev = to_net_dev(dev);
	ssize_t ret = -EINVAL;

	if (!rtnl_trylock())
		return restart_syscall();

	if (dev_isalive(netdev)) {
		struct netdev_phys_item_id ppid = { };

		ret = dev_get_port_parent_id(netdev, &ppid, false);
		if (!ret)
			ret = sprintf(buf, "%*phN\n", ppid.id_len, ppid.id);
	}
	rtnl_unlock();

	return ret;