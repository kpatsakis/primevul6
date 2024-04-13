
static ssize_t netdev_queue_attr_store(struct kobject *kobj,
				       struct attribute *attr,
				       const char *buf, size_t count)
{
	const struct netdev_queue_attribute *attribute
		= to_netdev_queue_attr(attr);
	struct netdev_queue *queue = to_netdev_queue(kobj);

	if (!attribute->store)
		return -EIO;

	return attribute->store(queue, buf, count);