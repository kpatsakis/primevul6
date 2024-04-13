static void rose_remove_route(struct rose_route *rose_route)
{
	struct rose_route *s;

	if (rose_route->neigh1 != NULL)
		rose_route->neigh1->use--;

	if (rose_route->neigh2 != NULL)
		rose_route->neigh2->use--;

	if ((s = rose_route_list) == rose_route) {
		rose_route_list = rose_route->next;
		kfree(rose_route);
		return;
	}

	while (s != NULL && s->next != NULL) {
		if (s->next == rose_route) {
			s->next = rose_route->next;
			kfree(rose_route);
			return;
		}

		s = s->next;
	}
}