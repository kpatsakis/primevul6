isdn_ppp_free(isdn_net_local *lp)
{
	struct ippp_struct *is;

	if (lp->ppp_slot < 0 || lp->ppp_slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "%s: ppp_slot(%d) out of range\n",
		       __func__, lp->ppp_slot);
		return 0;
	}

#ifdef CONFIG_ISDN_MPP
	spin_lock(&lp->netdev->pb->lock);
#endif
	isdn_net_rm_from_bundle(lp);
#ifdef CONFIG_ISDN_MPP
	if (lp->netdev->pb->ref_ct == 1)	/* last link in queue? */
		isdn_ppp_mp_cleanup(lp);

	lp->netdev->pb->ref_ct--;
	spin_unlock(&lp->netdev->pb->lock);
#endif /* CONFIG_ISDN_MPP */
	if (lp->ppp_slot < 0 || lp->ppp_slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "%s: ppp_slot(%d) now invalid\n",
		       __func__, lp->ppp_slot);
		return 0;
	}
	is = ippp_table[lp->ppp_slot];
	if ((is->state & IPPP_CONNECT))
		isdn_ppp_closewait(lp->ppp_slot);	/* force wakeup on ippp device */
	else if (is->state & IPPP_ASSIGNED)
		is->state = IPPP_OPEN;	/* fallback to 'OPEN but not ASSIGNED' state */

	if (is->debug & 0x1)
		printk(KERN_DEBUG "isdn_ppp_free %d %lx %lx\n", lp->ppp_slot, (long) lp, (long) is->lp);

	is->lp = NULL;          /* link is down .. set lp to NULL */
	lp->ppp_slot = -1;      /* is this OK ?? */

	return 0;
}