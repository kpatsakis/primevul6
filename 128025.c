#ifdef CONFIG_XPS
static ssize_t tx_maxrate_show(struct netdev_queue *queue,
			       char *buf)
{
	return sprintf(buf, "%lu\n", queue->tx_maxrate);