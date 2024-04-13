
static const void *netdev_queue_namespace(struct kobject *kobj)
{
	struct netdev_queue *queue = to_netdev_queue(kobj);
	struct device *dev = &queue->dev->dev;
	const void *ns = NULL;

	if (dev->class && dev->class->ns_type)
		ns = dev->class->namespace(dev);

	return ns;