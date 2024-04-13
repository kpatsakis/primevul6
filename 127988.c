
static ssize_t carrier_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	struct net_device *netdev = to_net_dev(dev);

	if (netif_running(netdev))
		return sprintf(buf, fmt_dec, !!netif_carrier_ok(netdev));

	return -EINVAL;