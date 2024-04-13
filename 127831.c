static struct ippp_ccp_reset_state *isdn_ppp_ccp_reset_alloc_state(struct ippp_struct *is,
								   unsigned char id)
{
	struct ippp_ccp_reset_state *rs;
	if (is->reset->rs[id]) {
		printk(KERN_WARNING "ippp_ccp: old state exists for id %d\n",
		       id);
		return NULL;
	} else {
		rs = kzalloc(sizeof(struct ippp_ccp_reset_state), GFP_KERNEL);
		if (!rs)
			return NULL;
		rs->state = CCPResetIdle;
		rs->is = is;
		rs->id = id;
		init_timer(&rs->timer);
		rs->timer.data = (unsigned long)rs;
		rs->timer.function = isdn_ppp_ccp_timer_callback;
		is->reset->rs[id] = rs;
	}
	return rs;
}