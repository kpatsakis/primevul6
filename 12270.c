void rose_link_device_down(struct net_device *dev)
{
	struct rose_neigh *rose_neigh;

	for (rose_neigh = rose_neigh_list; rose_neigh != NULL; rose_neigh = rose_neigh->next) {
		if (rose_neigh->dev == dev) {
			rose_del_route_by_neigh(rose_neigh);
			rose_kill_by_neigh(rose_neigh);
		}
	}
}