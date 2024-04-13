
static ssize_t name_assign_type_show(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
	struct net_device *ndev = to_net_dev(dev);
	ssize_t ret = -EINVAL;

	if (ndev->name_assign_type != NET_NAME_UNKNOWN)
		ret = netdev_show(dev, attr, buf, format_name_assign_type);

	return ret;