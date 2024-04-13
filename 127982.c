
static ssize_t ifalias_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	const struct net_device *netdev = to_net_dev(dev);
	char tmp[IFALIASZ];
	ssize_t ret = 0;

	ret = dev_get_alias(netdev, tmp, sizeof(tmp));
	if (ret > 0)
		ret = sprintf(buf, "%s\n", tmp);
	return ret;