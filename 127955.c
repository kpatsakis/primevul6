
static ssize_t rx_queue_attr_store(struct kobject *kobj, struct attribute *attr,
				   const char *buf, size_t count)
{
	const struct rx_queue_attribute *attribute = to_rx_queue_attr(attr);
	struct netdev_rx_queue *queue = to_rx_queue(kobj);

	if (!attribute->store)
		return -EIO;

	return attribute->store(queue, buf, count);