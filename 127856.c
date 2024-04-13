static void isdn_ppp_ccp_timer_callback(unsigned long closure)
{
	struct ippp_ccp_reset_state *rs =
		(struct ippp_ccp_reset_state *)closure;

	if (!rs) {
		printk(KERN_ERR "ippp_ccp: timer cb with zero closure.\n");
		return;
	}
	if (rs->ta && rs->state == CCPResetSentReq) {
		/* We are correct here */
		if (!rs->expra) {
			/* Hmm, there is no Ack really expected. We can clean
			   up the state now, it will be reallocated if the
			   decompressor insists on another reset */
			rs->ta = 0;
			isdn_ppp_ccp_reset_free_state(rs->is, rs->id);
			return;
		}
		printk(KERN_DEBUG "ippp_ccp: CCP Reset timed out for id %d\n",
		       rs->id);
		/* Push it again */
		isdn_ppp_ccp_xmit_reset(rs->is, PPP_CCP, CCP_RESETREQ, rs->id,
					rs->data, rs->dlen);
		/* Restart timer */
		rs->timer.expires = jiffies + HZ * 5;
		add_timer(&rs->timer);
	} else {
		printk(KERN_WARNING "ippp_ccp: timer cb in wrong state %d\n",
		       rs->state);
	}
}