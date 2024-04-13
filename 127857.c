static struct ippp_ccp_reset *isdn_ppp_ccp_reset_alloc(struct ippp_struct *is)
{
	struct ippp_ccp_reset *r;
	r = kzalloc(sizeof(struct ippp_ccp_reset), GFP_KERNEL);
	if (!r) {
		printk(KERN_ERR "ippp_ccp: failed to allocate reset data"
		       " structure - no mem\n");
		return NULL;
	}
	printk(KERN_DEBUG "ippp_ccp: allocated reset data structure %p\n", r);
	is->reset = r;
	return r;
}