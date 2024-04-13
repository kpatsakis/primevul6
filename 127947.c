
static ssize_t speed_show(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct net_device *netdev = to_net_dev(dev);
	int ret = -EINVAL;

	if (!rtnl_trylock())
		return restart_syscall();

	if (netif_running(netdev)) {
		struct ethtool_link_ksettings cmd;

		if (!__ethtool_get_link_ksettings(netdev, &cmd))
			ret = sprintf(buf, fmt_dec, cmd.base.speed);
	}
	rtnl_unlock();
	return ret;