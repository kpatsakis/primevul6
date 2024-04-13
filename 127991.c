
static ssize_t group_store(struct device *dev, struct device_attribute *attr,
			   const char *buf, size_t len)
{
	return netdev_store(dev, attr, buf, len, change_group);