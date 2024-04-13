static void *rose_route_next(struct seq_file *seq, void *v, loff_t *pos)
{
	++*pos;

	return (v == SEQ_START_TOKEN) ? rose_route_list
		: ((struct rose_route *)v)->next;
}