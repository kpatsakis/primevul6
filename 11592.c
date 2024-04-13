cleanup(void)
{
    outs(exit_attribute_mode);
    if (!outs(orig_colors))
	outs(orig_pair);
    outs(clear_screen);
    outs(cursor_normal);

    fflush(stdout);
    fprintf(stderr, "\n\n%ld total cells, rate %.2f/sec\n",
	    total_chars,
	    ((double) (total_chars) / (double) (time((time_t *) 0) - started)));
}