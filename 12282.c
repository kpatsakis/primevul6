static int rose_dev_exists(rose_address *addr)
{
	struct net_device *dev;

	rcu_read_lock();
	for_each_netdev_rcu(&init_net, dev) {
		if ((dev->flags & IFF_UP) && dev->type == ARPHRD_ROSE &&
		    rosecmp(addr, (const rose_address *)dev->dev_addr) == 0)
			goto out;
	}
	dev = NULL;
out:
	rcu_read_unlock();
	return dev != NULL;
}