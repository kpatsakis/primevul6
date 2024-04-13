void rose_rt_device_down(struct net_device *dev)
{
	struct rose_neigh *s, *rose_neigh;
	struct rose_node  *t, *rose_node;
	int i;

	spin_lock_bh(&rose_node_list_lock);
	spin_lock_bh(&rose_neigh_list_lock);
	rose_neigh = rose_neigh_list;
	while (rose_neigh != NULL) {
		s          = rose_neigh;
		rose_neigh = rose_neigh->next;

		if (s->dev != dev)
			continue;

		rose_node = rose_node_list;

		while (rose_node != NULL) {
			t         = rose_node;
			rose_node = rose_node->next;

			for (i = 0; i < t->count; i++) {
				if (t->neighbour[i] != s)
					continue;

				t->count--;

				switch (i) {
				case 0:
					t->neighbour[0] = t->neighbour[1];
					fallthrough;
				case 1:
					t->neighbour[1] = t->neighbour[2];
					break;
				case 2:
					break;
				}
			}

			if (t->count <= 0)
				rose_remove_node(t);
		}

		rose_remove_neigh(s);
	}
	spin_unlock_bh(&rose_neigh_list_lock);
	spin_unlock_bh(&rose_node_list_lock);
}