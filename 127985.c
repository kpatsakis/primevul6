#ifdef CONFIG_XPS
static ssize_t xps_cpus_show(struct netdev_queue *queue,
			     char *buf)
{
	struct net_device *dev = queue->dev;
	int cpu, len, num_tc = 1, tc = 0;
	struct xps_dev_maps *dev_maps;
	cpumask_var_t mask;
	unsigned long index;

	if (!netif_is_multiqueue(dev))
		return -ENOENT;

	index = get_netdev_queue_index(queue);

	if (dev->num_tc) {
		/* Do not allow XPS on subordinate device directly */
		num_tc = dev->num_tc;
		if (num_tc < 0)
			return -EINVAL;

		/* If queue belongs to subordinate dev use its map */
		dev = netdev_get_tx_queue(dev, index)->sb_dev ? : dev;

		tc = netdev_txq_to_tc(dev, index);
		if (tc < 0)
			return -EINVAL;
	}

	if (!zalloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	rcu_read_lock();
	dev_maps = rcu_dereference(dev->xps_cpus_map);
	if (dev_maps) {
		for_each_possible_cpu(cpu) {
			int i, tci = cpu * num_tc + tc;
			struct xps_map *map;

			map = rcu_dereference(dev_maps->attr_map[tci]);
			if (!map)
				continue;

			for (i = map->len; i--;) {
				if (map->queues[i] == index) {
					cpumask_set_cpu(cpu, mask);
					break;
				}
			}
		}
	}
	rcu_read_unlock();

	len = snprintf(buf, PAGE_SIZE, "%*pb\n", cpumask_pr_args(mask));
	free_cpumask_var(mask);
	return len < PAGE_SIZE ? len : -EINVAL;