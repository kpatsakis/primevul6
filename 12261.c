struct net_device *rose_dev_first(void)
{
	struct net_device *dev, *first = NULL;

	rcu_read_lock();
	for_each_netdev_rcu(&init_net, dev) {
		if ((dev->flags & IFF_UP) && dev->type == ARPHRD_ROSE)
			if (first == NULL || strncmp(dev->name, first->name, 3) < 0)
				first = dev;
	}
	rcu_read_unlock();

	return first;
}