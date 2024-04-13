
static ssize_t tx_maxrate_store(struct netdev_queue *queue,
				const char *buf, size_t len)
{
	struct net_device *dev = queue->dev;
	int err, index = get_netdev_queue_index(queue);
	u32 rate = 0;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	err = kstrtou32(buf, 10, &rate);
	if (err < 0)
		return err;

	if (!rtnl_trylock())
		return restart_syscall();

	err = -EOPNOTSUPP;
	if (dev->netdev_ops->ndo_set_tx_maxrate)
		err = dev->netdev_ops->ndo_set_tx_maxrate(dev, index, rate);

	rtnl_unlock();
	if (!err) {
		queue->tx_maxrate = rate;
		return len;
	}
	return err;