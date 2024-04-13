cleanup(void)
{
    outs(t_me);
    if (!outs(t_oc))
	outs(t_op);
    outs(t_cl);
    outs(t_ve);

    fflush(stdout);
    fprintf(stderr, "\n\n%ld total cells, rate %.2f/sec\n",
	    total_chars,
	    ((double) (total_chars) / (double) (time((time_t *) 0) - started)));
}