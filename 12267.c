static int rose_node_show(struct seq_file *seq, void *v)
{
	char rsbuf[11];
	int i;

	if (v == SEQ_START_TOKEN)
		seq_puts(seq, "address    mask n neigh neigh neigh\n");
	else {
		const struct rose_node *rose_node = v;
		seq_printf(seq, "%-10s %04d %d",
			   rose2asc(rsbuf, &rose_node->address),
			   rose_node->mask,
			   rose_node->count);

		for (i = 0; i < rose_node->count; i++)
			seq_printf(seq, " %05d", rose_node->neighbour[i]->number);

		seq_puts(seq, "\n");
	}
	return 0;
}