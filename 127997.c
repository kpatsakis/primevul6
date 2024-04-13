
static ssize_t gro_flush_timeout_store(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf, size_t len)
{
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	return netdev_store(dev, attr, buf, len, change_gro_flush_timeout);