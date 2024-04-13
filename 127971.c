#ifdef CONFIG_RPS
static ssize_t show_rps_map(struct netdev_rx_queue *queue, char *buf)
{
	struct rps_map *map;
	cpumask_var_t mask;
	int i, len;

	if (!zalloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	rcu_read_lock();
	map = rcu_dereference(queue->rps_map);
	if (map)
		for (i = 0; i < map->len; i++)
			cpumask_set_cpu(map->cpus[i], mask);

	len = snprintf(buf, PAGE_SIZE, "%*pb\n", cpumask_pr_args(mask));
	rcu_read_unlock();
	free_cpumask_var(mask);

	return len < PAGE_SIZE ? len : -EINVAL;