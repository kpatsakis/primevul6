
static ssize_t duplex_show(struct device *dev,
			   struct device_attribute *attr, char *buf)
{
	struct net_device *netdev = to_net_dev(dev);
	int ret = -EINVAL;

	if (!rtnl_trylock())
		return restart_syscall();

	if (netif_running(netdev)) {
		struct ethtool_link_ksettings cmd;

		if (!__ethtool_get_link_ksettings(netdev, &cmd)) {
			const char *duplex;

			switch (cmd.base.duplex) {
			case DUPLEX_HALF:
				duplex = "half";
				break;
			case DUPLEX_FULL:
				duplex = "full";
				break;
			default:
				duplex = "unknown";
				break;
			}
			ret = sprintf(buf, "%s\n", duplex);
		}
	}
	rtnl_unlock();
	return ret;