isdn_ppp_init(void)
{
	int i,
		j;

#ifdef CONFIG_ISDN_MPP
	if (isdn_ppp_mp_bundle_array_init() < 0)
		return -ENOMEM;
#endif /* CONFIG_ISDN_MPP */

	for (i = 0; i < ISDN_MAX_CHANNELS; i++) {
		if (!(ippp_table[i] = kzalloc(sizeof(struct ippp_struct), GFP_KERNEL))) {
			printk(KERN_WARNING "isdn_ppp_init: Could not alloc ippp_table\n");
			for (j = 0; j < i; j++)
				kfree(ippp_table[j]);
			return -1;
		}
		spin_lock_init(&ippp_table[i]->buflock);
		ippp_table[i]->state = 0;
		ippp_table[i]->first = ippp_table[i]->rq + NUM_RCV_BUFFS - 1;
		ippp_table[i]->last = ippp_table[i]->rq;

		for (j = 0; j < NUM_RCV_BUFFS; j++) {
			ippp_table[i]->rq[j].buf = NULL;
			ippp_table[i]->rq[j].last = ippp_table[i]->rq +
				(NUM_RCV_BUFFS + j - 1) % NUM_RCV_BUFFS;
			ippp_table[i]->rq[j].next = ippp_table[i]->rq + (j + 1) % NUM_RCV_BUFFS;
		}
	}
	return 0;
}