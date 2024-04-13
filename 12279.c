void rose_add_loopback_neigh(void)
{
	struct rose_neigh *sn;

	rose_loopback_neigh = kmalloc(sizeof(struct rose_neigh), GFP_KERNEL);
	if (!rose_loopback_neigh)
		return;
	sn = rose_loopback_neigh;

	sn->callsign  = null_ax25_address;
	sn->digipeat  = NULL;
	sn->ax25      = NULL;
	sn->dev       = NULL;
	sn->count     = 0;
	sn->use       = 0;
	sn->dce_mode  = 1;
	sn->loopback  = 1;
	sn->number    = rose_neigh_no++;
	sn->restarted = 1;

	skb_queue_head_init(&sn->queue);

	timer_setup(&sn->ftimer, NULL, 0);
	timer_setup(&sn->t0timer, NULL, 0);

	spin_lock_bh(&rose_neigh_list_lock);
	sn->next = rose_neigh_list;
	rose_neigh_list           = sn;
	spin_unlock_bh(&rose_neigh_list_lock);
}