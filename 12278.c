static int rose_neigh_show(struct seq_file *seq, void *v)
{
	char buf[11];
	int i;

	if (v == SEQ_START_TOKEN)
		seq_puts(seq,
			 "addr  callsign  dev  count use mode restart  t0  tf digipeaters\n");
	else {
		struct rose_neigh *rose_neigh = v;

		/* if (!rose_neigh->loopback) { */
		seq_printf(seq, "%05d %-9s %-4s   %3d %3d  %3s     %3s %3lu %3lu",
			   rose_neigh->number,
			   (rose_neigh->loopback) ? "RSLOOP-0" : ax2asc(buf, &rose_neigh->callsign),
			   rose_neigh->dev ? rose_neigh->dev->name : "???",
			   rose_neigh->count,
			   rose_neigh->use,
			   (rose_neigh->dce_mode) ? "DCE" : "DTE",
			   (rose_neigh->restarted) ? "yes" : "no",
			   ax25_display_timer(&rose_neigh->t0timer) / HZ,
			   ax25_display_timer(&rose_neigh->ftimer)  / HZ);

		if (rose_neigh->digipeat != NULL) {
			for (i = 0; i < rose_neigh->digipeat->ndigi; i++)
				seq_printf(seq, " %s", ax2asc(buf, &rose_neigh->digipeat->calls[i]));
		}

		seq_puts(seq, "\n");
	}
	return 0;
}