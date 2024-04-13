isdn_ppp_bundle(struct ippp_struct *is, int unit)
{
	char ifn[IFNAMSIZ + 1];
	isdn_net_dev *p;
	isdn_net_local *lp, *nlp;
	int rc;
	unsigned long flags;

	sprintf(ifn, "ippp%d", unit);
	p = isdn_net_findif(ifn);
	if (!p) {
		printk(KERN_ERR "ippp_bundle: cannot find %s\n", ifn);
		return -EINVAL;
	}

	spin_lock_irqsave(&p->pb->lock, flags);

	nlp = is->lp;
	lp = p->queue;
	if (nlp->ppp_slot < 0 || nlp->ppp_slot >= ISDN_MAX_CHANNELS ||
	    lp->ppp_slot < 0 || lp->ppp_slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "ippp_bundle: binding to invalid slot %d\n",
		       nlp->ppp_slot < 0 || nlp->ppp_slot >= ISDN_MAX_CHANNELS ?
		       nlp->ppp_slot : lp->ppp_slot);
		rc = -EINVAL;
		goto out;
	}

	isdn_net_add_to_bundle(p, nlp);

	ippp_table[nlp->ppp_slot]->unit = ippp_table[lp->ppp_slot]->unit;

	/* maybe also SC_CCP stuff */
	ippp_table[nlp->ppp_slot]->pppcfg |= ippp_table[lp->ppp_slot]->pppcfg &
		(SC_ENABLE_IP | SC_NO_TCP_CCID | SC_REJ_COMP_TCP);
	ippp_table[nlp->ppp_slot]->mpppcfg |= ippp_table[lp->ppp_slot]->mpppcfg &
		(SC_MP_PROT | SC_REJ_MP_PROT | SC_OUT_SHORT_SEQ | SC_IN_SHORT_SEQ);
	rc = isdn_ppp_mp_init(nlp, p->pb);
out:
	spin_unlock_irqrestore(&p->pb->lock, flags);
	return rc;
}