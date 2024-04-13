static void *rose_node_next(struct seq_file *seq, void *v, loff_t *pos)
{
	++*pos;

	return (v == SEQ_START_TOKEN) ? rose_node_list
		: ((struct rose_node *)v)->next;
}