
static ssize_t xps_rxqs_show(struct netdev_queue *queue, char *buf)
{
	struct net_device *dev = queue->dev;
	struct xps_dev_maps *dev_maps;
	unsigned long *mask, index;
	int j, len, num_tc = 1, tc = 0;

	index = get_netdev_queue_index(queue);

	if (dev->num_tc) {
		num_tc = dev->num_tc;
		tc = netdev_txq_to_tc(dev, index);
		if (tc < 0)
			return -EINVAL;
	}
	mask = bitmap_zalloc(dev->num_rx_queues, GFP_KERNEL);
	if (!mask)
		return -ENOMEM;

	rcu_read_lock();
	dev_maps = rcu_dereference(dev->xps_rxqs_map);
	if (!dev_maps)
		goto out_no_maps;

	for (j = -1; j = netif_attrmask_next(j, NULL, dev->num_rx_queues),
	     j < dev->num_rx_queues;) {
		int i, tci = j * num_tc + tc;
		struct xps_map *map;

		map = rcu_dereference(dev_maps->attr_map[tci]);
		if (!map)
			continue;

		for (i = map->len; i--;) {
			if (map->queues[i] == index) {
				set_bit(j, mask);
				break;
			}
		}
	}
out_no_maps:
	rcu_read_unlock();

	len = bitmap_print_to_pagebuf(false, buf, mask, dev->num_rx_queues);
	bitmap_free(mask);

	return len < PAGE_SIZE ? len : -EINVAL;