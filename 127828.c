isdn_ppp_wakeup_daemon(isdn_net_local *lp)
{
	if (lp->ppp_slot < 0 || lp->ppp_slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "%s: ppp_slot(%d) out of range\n",
		       __func__, lp->ppp_slot);
		return;
	}
	ippp_table[lp->ppp_slot]->state = IPPP_OPEN | IPPP_CONNECT | IPPP_NOBLOCK;
	wake_up_interruptible(&ippp_table[lp->ppp_slot]->wq);
}