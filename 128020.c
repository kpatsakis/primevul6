
static ssize_t rx_queue_attr_show(struct kobject *kobj, struct attribute *attr,
				  char *buf)
{
	const struct rx_queue_attribute *attribute = to_rx_queue_attr(attr);
	struct netdev_rx_queue *queue = to_rx_queue(kobj);

	if (!attribute->show)
		return -EIO;

	return attribute->show(queue, buf);