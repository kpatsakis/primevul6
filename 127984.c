
static void rx_queue_release(struct kobject *kobj)
{
	struct netdev_rx_queue *queue = to_rx_queue(kobj);
#ifdef CONFIG_RPS
	struct rps_map *map;
	struct rps_dev_flow_table *flow_table;

	map = rcu_dereference_protected(queue->rps_map, 1);
	if (map) {
		RCU_INIT_POINTER(queue->rps_map, NULL);
		kfree_rcu(map, rcu);
	}

	flow_table = rcu_dereference_protected(queue->rps_flow_table, 1);
	if (flow_table) {
		RCU_INIT_POINTER(queue->rps_flow_table, NULL);
		call_rcu(&flow_table->rcu, rps_dev_flow_table_release);
	}
#endif

	memset(kobj, 0, sizeof(*kobj));
	dev_put(queue->dev);