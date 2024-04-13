static void *rose_neigh_next(struct seq_file *seq, void *v, loff_t *pos)
{
	++*pos;

	return (v == SEQ_START_TOKEN) ? rose_neigh_list
		: ((struct rose_neigh *)v)->next;
}