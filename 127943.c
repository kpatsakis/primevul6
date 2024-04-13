
static ssize_t broadcast_show(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	struct net_device *ndev = to_net_dev(dev);

	if (dev_isalive(ndev))
		return sysfs_format_mac(buf, ndev->broadcast, ndev->addr_len);
	return -EINVAL;