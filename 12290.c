	__acquires(rose_route_list_lock)
{
	struct rose_route *rose_route;
	int i = 1;

	spin_lock_bh(&rose_route_list_lock);
	if (*pos == 0)
		return SEQ_START_TOKEN;

	for (rose_route = rose_route_list; rose_route && i < *pos;
	     rose_route = rose_route->next, ++i);

	return (i == *pos) ? rose_route : NULL;
}