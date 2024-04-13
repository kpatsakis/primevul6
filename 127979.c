
static const void *rx_queue_namespace(struct kobject *kobj)
{
	struct netdev_rx_queue *queue = to_rx_queue(kobj);
	struct device *dev = &queue->dev->dev;
	const void *ns = NULL;

	if (dev->class && dev->class->ns_type)
		ns = dev->class->namespace(dev);

	return ns;