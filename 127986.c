
static ssize_t tx_timeout_show(struct netdev_queue *queue, char *buf)
{
	unsigned long trans_timeout;

	spin_lock_irq(&queue->_xmit_lock);
	trans_timeout = queue->trans_timeout;
	spin_unlock_irq(&queue->_xmit_lock);

	return sprintf(buf, "%lu", trans_timeout);