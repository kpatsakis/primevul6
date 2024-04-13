static int rose_route_show(struct seq_file *seq, void *v)
{
	char buf[11], rsbuf[11];

	if (v == SEQ_START_TOKEN)
		seq_puts(seq,
			 "lci  address     callsign   neigh  <-> lci  address     callsign   neigh\n");
	else {
		struct rose_route *rose_route = v;

		if (rose_route->neigh1)
			seq_printf(seq,
				   "%3.3X  %-10s  %-9s  %05d      ",
				   rose_route->lci1,
				   rose2asc(rsbuf, &rose_route->src_addr),
				   ax2asc(buf, &rose_route->src_call),
				   rose_route->neigh1->number);
		else
			seq_puts(seq,
				 "000  *           *          00000      ");

		if (rose_route->neigh2)
			seq_printf(seq,
				   "%3.3X  %-10s  %-9s  %05d\n",
				   rose_route->lci2,
				   rose2asc(rsbuf, &rose_route->dest_addr),
				   ax2asc(buf, &rose_route->dest_call),
				   rose_route->neigh2->number);
		 else
			 seq_puts(seq,
				  "000  *           *          00000\n");
		}
	return 0;
}