cleanup(void)
{
    endwin();

    fflush(stdout);
    fprintf(stderr, "\n\n%ld total cells, rate %.2f/sec\n",
	    total_chars,
	    ((double) (total_chars) / (double) (time((time_t *) 0) - started)));
}