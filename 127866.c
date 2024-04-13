static void isdn_ppp_ccp_reset_ack_rcvd(struct ippp_struct *is,
					unsigned char id)
{
	struct ippp_ccp_reset_state *rs = is->reset->rs[id];

	if (rs) {
		if (rs->ta && rs->state == CCPResetSentReq) {
			/* Great, we are correct */
			if (!rs->expra)
				printk(KERN_DEBUG "ippp_ccp: ResetAck received"
				       " for id %d but not expected\n", id);
		} else {
			printk(KERN_INFO "ippp_ccp: ResetAck received out of"
			       "sync for id %d\n", id);
		}
		if (rs->ta) {
			rs->ta = 0;
			del_timer(&rs->timer);
		}
		isdn_ppp_ccp_reset_free_state(is, id);
	} else {
		printk(KERN_INFO "ippp_ccp: ResetAck received for unknown id"
		       " %d\n", id);
	}
	/* Make sure the simple reset stuff uses a new id next time */
	is->reset->lastid++;
}