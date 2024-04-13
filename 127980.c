
static ssize_t carrier_up_count_show(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
	struct net_device *netdev = to_net_dev(dev);

	return sprintf(buf, fmt_dec, atomic_read(&netdev->carrier_up_count));