void rose_link_failed(ax25_cb *ax25, int reason)
{
	struct rose_neigh *rose_neigh;

	spin_lock_bh(&rose_neigh_list_lock);
	rose_neigh = rose_neigh_list;
	while (rose_neigh != NULL) {
		if (rose_neigh->ax25 == ax25)
			break;
		rose_neigh = rose_neigh->next;
	}

	if (rose_neigh != NULL) {
		rose_neigh->ax25 = NULL;
		ax25_cb_put(ax25);

		rose_del_route_by_neigh(rose_neigh);
		rose_kill_by_neigh(rose_neigh);
	}
	spin_unlock_bh(&rose_neigh_list_lock);
}