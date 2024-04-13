static void isdn_ppp_ccp_reset_free_state(struct ippp_struct *is,
					  unsigned char id)
{
	struct ippp_ccp_reset_state *rs;

	if (is->reset->rs[id]) {
		printk(KERN_DEBUG "ippp_ccp: freeing state for id %d\n", id);
		rs = is->reset->rs[id];
		/* Make sure the kernel will not call back later */
		if (rs->ta)
			del_timer(&rs->timer);
		is->reset->rs[id] = NULL;
		kfree(rs);
	} else {
		printk(KERN_WARNING "ippp_ccp: id %d is not allocated\n", id);
	}
}