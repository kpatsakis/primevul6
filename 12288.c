	__acquires(rose_node_list_lock)
{
	struct rose_node *rose_node;
	int i = 1;

	spin_lock_bh(&rose_node_list_lock);
	if (*pos == 0)
		return SEQ_START_TOKEN;

	for (rose_node = rose_node_list; rose_node && i < *pos;
	     rose_node = rose_node->next, ++i);

	return (i == *pos) ? rose_node : NULL;
}