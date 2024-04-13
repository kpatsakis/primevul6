
static void remove_queue_kobjects(struct net_device *dev)
{
	int real_rx = 0, real_tx = 0;

#ifdef CONFIG_SYSFS
	real_rx = dev->real_num_rx_queues;
#endif
	real_tx = dev->real_num_tx_queues;

	net_rx_queue_update_kobjects(dev, real_rx, 0);
	netdev_queue_update_kobjects(dev, real_tx, 0);
#ifdef CONFIG_SYSFS
	kset_unregister(dev->queues_kset);
#endif