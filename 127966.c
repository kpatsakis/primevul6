
static ssize_t mtu_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t len)
{
	return netdev_store(dev, attr, buf, len, change_mtu);