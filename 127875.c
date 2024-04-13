static void isdn_ppp_ccp_reset_free(struct ippp_struct *is)
{
	unsigned int id;

	printk(KERN_DEBUG "ippp_ccp: freeing reset data structure %p\n",
	       is->reset);
	for (id = 0; id < 256; id++) {
		if (is->reset->rs[id]) {
			isdn_ppp_ccp_reset_free_state(is, (unsigned char)id);
		}
	}
	kfree(is->reset);
	is->reset = NULL;
}