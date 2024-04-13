change_winsz(void)
{
	struct timeval tv;

	mksh_TIME(tv);
	BAFHUpdateMem_mem(qh_state, &tv, sizeof(tv));

#ifdef TIOCGWINSZ
	/* check if window size has changed */
	if (tty_init_fd() < 2) {
		struct winsize ws;

		if (ioctl(tty_fd, TIOCGWINSZ, &ws) >= 0) {
			if (ws.ws_col)
				x_cols = ws.ws_col;
			if (ws.ws_row)
				x_lins = ws.ws_row;
		}
	}
#endif

	/* bounds check for sane values, use defaults otherwise */
	if (x_cols < MIN_COLS)
		x_cols = 80;
	if (x_lins < MIN_LINS)
		x_lins = 24;

#ifdef SIGWINCH
	got_winch = 0;
#endif
}