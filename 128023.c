
static unsigned int get_netdev_queue_index(struct netdev_queue *queue)
{
	struct net_device *dev = queue->dev;
	unsigned int i;

	i = queue - dev->_tx;
	BUG_ON(i >= dev->num_tx_queues);

	return i;