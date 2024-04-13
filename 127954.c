
static ssize_t xps_cpus_store(struct netdev_queue *queue,
			      const char *buf, size_t len)
{
	struct net_device *dev = queue->dev;
	unsigned long index;
	cpumask_var_t mask;
	int err;

	if (!netif_is_multiqueue(dev))
		return -ENOENT;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (!alloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	index = get_netdev_queue_index(queue);

	err = bitmap_parse(buf, len, cpumask_bits(mask), nr_cpumask_bits);
	if (err) {
		free_cpumask_var(mask);
		return err;
	}

	err = netif_set_xps_queue(dev, mask, index);

	free_cpumask_var(mask);

	return err ? : len;