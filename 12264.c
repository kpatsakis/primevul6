struct rose_route *rose_route_free_lci(unsigned int lci, struct rose_neigh *neigh)
{
	struct rose_route *rose_route;

	for (rose_route = rose_route_list; rose_route != NULL; rose_route = rose_route->next)
		if ((rose_route->neigh1 == neigh && rose_route->lci1 == lci) ||
		    (rose_route->neigh2 == neigh && rose_route->lci2 == lci))
			return rose_route;

	return NULL;
}