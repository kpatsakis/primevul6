	__acquires(rose_neigh_list_lock)
{
	struct rose_neigh *rose_neigh;
	int i = 1;

	spin_lock_bh(&rose_neigh_list_lock);
	if (*pos == 0)
		return SEQ_START_TOKEN;

	for (rose_neigh = rose_neigh_list; rose_neigh && i < *pos;
	     rose_neigh = rose_neigh->next, ++i);

	return (i == *pos) ? rose_neigh : NULL;
}