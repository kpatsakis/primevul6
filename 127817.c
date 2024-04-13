static int isdn_ppp_mp_init(isdn_net_local *lp, ippp_bundle *add_to)
{
	struct ippp_struct *is;

	if (lp->ppp_slot < 0) {
		printk(KERN_ERR "%s: lp->ppp_slot(%d) out of range\n",
		       __func__, lp->ppp_slot);
		return (-EINVAL);
	}

	is = ippp_table[lp->ppp_slot];
	if (add_to) {
		if (lp->netdev->pb)
			lp->netdev->pb->ref_ct--;
		lp->netdev->pb = add_to;
	} else {		/* first link in a bundle */
		is->mp_seqno = 0;
		if ((lp->netdev->pb = isdn_ppp_mp_bundle_alloc()) == NULL)
			return -ENOMEM;
		lp->next = lp->last = lp;	/* nobody else in a queue */
		lp->netdev->pb->frags = NULL;
		lp->netdev->pb->frames = 0;
		lp->netdev->pb->seq = UINT_MAX;
	}
	lp->netdev->pb->ref_ct++;

	is->last_link_seqno = 0;
	return 0;
}