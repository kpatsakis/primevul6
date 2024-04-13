
static ssize_t traffic_class_show(struct netdev_queue *queue,
				  char *buf)
{
	struct net_device *dev = queue->dev;
	int index;
	int tc;

	if (!netif_is_multiqueue(dev))
		return -ENOENT;

	index = get_netdev_queue_index(queue);

	/* If queue belongs to subordinate dev use its TC mapping */
	dev = netdev_get_tx_queue(dev, index)->sb_dev ? : dev;

	tc = netdev_txq_to_tc(dev, index);
	if (tc < 0)
		return -EINVAL;

	/* We can report the traffic class one of two ways:
	 * Subordinate device traffic classes are reported with the traffic
	 * class first, and then the subordinate class so for example TC0 on
	 * subordinate device 2 will be reported as "0-2". If the queue
	 * belongs to the root device it will be reported with just the
	 * traffic class, so just "0" for TC 0 for example.
	 */
	return dev->num_tc < 0 ? sprintf(buf, "%u%d\n", tc, dev->num_tc) :
				 sprintf(buf, "%u\n", tc);