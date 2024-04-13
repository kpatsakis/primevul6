
static ssize_t tx_queue_len_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t len)
{
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	return netdev_store(dev, attr, buf, len, dev_change_tx_queue_len);