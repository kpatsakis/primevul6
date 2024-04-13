void rose_route_device_down(struct net_device *dev)
{
	struct rose_route *s, *rose_route;

	spin_lock_bh(&rose_route_list_lock);
	rose_route = rose_route_list;
	while (rose_route != NULL) {
		s          = rose_route;
		rose_route = rose_route->next;

		if (s->neigh1->dev == dev || s->neigh2->dev == dev)
			rose_remove_route(s);
	}
	spin_unlock_bh(&rose_route_list_lock);
}