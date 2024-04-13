static int rose_clear_routes(void)
{
	struct rose_neigh *s, *rose_neigh;
	struct rose_node  *t, *rose_node;

	spin_lock_bh(&rose_node_list_lock);
	spin_lock_bh(&rose_neigh_list_lock);

	rose_neigh = rose_neigh_list;
	rose_node  = rose_node_list;

	while (rose_node != NULL) {
		t         = rose_node;
		rose_node = rose_node->next;
		if (!t->loopback)
			rose_remove_node(t);
	}

	while (rose_neigh != NULL) {
		s          = rose_neigh;
		rose_neigh = rose_neigh->next;

		if (s->use == 0 && !s->loopback) {
			s->count = 0;
			rose_remove_neigh(s);
		}
	}

	spin_unlock_bh(&rose_neigh_list_lock);
	spin_unlock_bh(&rose_node_list_lock);

	return 0;
}