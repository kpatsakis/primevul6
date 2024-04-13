static int sp_close(struct net_device *dev)
{
	struct sixpack *sp = netdev_priv(dev);

	spin_lock_bh(&sp->lock);
	if (sp->tty) {
		/* TTY discipline is running. */
		clear_bit(TTY_DO_WRITE_WAKEUP, &sp->tty->flags);
	}
	netif_stop_queue(dev);
	spin_unlock_bh(&sp->lock);

	return 0;
}