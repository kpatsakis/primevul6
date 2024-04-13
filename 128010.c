
static ssize_t netdev_queue_attr_show(struct kobject *kobj,
				      struct attribute *attr, char *buf)
{
	const struct netdev_queue_attribute *attribute
		= to_netdev_queue_attr(attr);
	struct netdev_queue *queue = to_netdev_queue(kobj);

	if (!attribute->show)
		return -EIO;

	return attribute->show(queue, buf);