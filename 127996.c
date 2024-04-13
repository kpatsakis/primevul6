
static void netdev_queue_release(struct kobject *kobj)
{
	struct netdev_queue *queue = to_netdev_queue(kobj);

	memset(kobj, 0, sizeof(*kobj));
	dev_put(queue->dev);