isdn_ppp_bind(isdn_net_local *lp)
{
	int i;
	int unit = 0;
	struct ippp_struct *is;
	int retval;

	if (lp->pppbind < 0) {  /* device bounded to ippp device ? */
		isdn_net_dev *net_dev = dev->netdev;
		char exclusive[ISDN_MAX_CHANNELS];	/* exclusive flags */
		memset(exclusive, 0, ISDN_MAX_CHANNELS);
		while (net_dev) {	/* step through net devices to find exclusive minors */
			isdn_net_local *lp = net_dev->local;
			if (lp->pppbind >= 0)
				exclusive[lp->pppbind] = 1;
			net_dev = net_dev->next;
		}
		/*
		 * search a free device / slot
		 */
		for (i = 0; i < ISDN_MAX_CHANNELS; i++) {
			if (ippp_table[i]->state == IPPP_OPEN && !exclusive[ippp_table[i]->minor]) {	/* OPEN, but not connected! */
				break;
			}
		}
	} else {
		for (i = 0; i < ISDN_MAX_CHANNELS; i++) {
			if (ippp_table[i]->minor == lp->pppbind &&
			    (ippp_table[i]->state & IPPP_OPEN) == IPPP_OPEN)
				break;
		}
	}

	if (i >= ISDN_MAX_CHANNELS) {
		printk(KERN_WARNING "isdn_ppp_bind: Can't find a (free) connection to the ipppd daemon.\n");
		retval = -1;
		goto out;
	}
	/* get unit number from interface name .. ugly! */
	unit = isdn_ppp_if_get_unit(lp->netdev->dev->name);
	if (unit < 0) {
		printk(KERN_ERR "isdn_ppp_bind: illegal interface name %s.\n",
		       lp->netdev->dev->name);
		retval = -1;
		goto out;
	}

	lp->ppp_slot = i;
	is = ippp_table[i];
	is->lp = lp;
	is->unit = unit;
	is->state = IPPP_OPEN | IPPP_ASSIGNED;	/* assigned to a netdevice but not connected */
#ifdef CONFIG_ISDN_MPP
	retval = isdn_ppp_mp_init(lp, NULL);
	if (retval < 0)
		goto out;
#endif /* CONFIG_ISDN_MPP */

	retval = lp->ppp_slot;

out:
	return retval;
}