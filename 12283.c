void __exit rose_rt_free(void)
{
	struct rose_neigh *s, *rose_neigh = rose_neigh_list;
	struct rose_node  *t, *rose_node  = rose_node_list;
	struct rose_route *u, *rose_route = rose_route_list;

	while (rose_neigh != NULL) {
		s          = rose_neigh;
		rose_neigh = rose_neigh->next;

		rose_remove_neigh(s);
	}

	while (rose_node != NULL) {
		t         = rose_node;
		rose_node = rose_node->next;

		rose_remove_node(t);
	}

	while (rose_route != NULL) {
		u          = rose_route;
		rose_route = rose_route->next;

		rose_remove_route(u);
	}
}