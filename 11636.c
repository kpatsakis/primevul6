main(int argc GCC_UNUSED, char *argv[]GCC_UNUSED)
{
    _nc_STRCPY(tname, getenv("TERM"), sizeof(tname));
    load_term();
    _nc_setupscreen(lines, columns, stdout, FALSE, 0);
    baudrate();

    _nc_mvcur_init();

    (void) puts("The mvcur tester.  Type ? for help");

    fputs("smcup:", stdout);
    putchar('\n');

    for (;;) {
	int fy, fx, ty, tx, n, i;
	char buf[BUFSIZ], capname[BUFSIZ];

	if (fputs("> ", stdout) == EOF)
	    break;
	if (fgets(buf, sizeof(buf), stdin) == 0)
	    break;

#define PUTS(s)   (void) puts(s)
#define PUTF(s,t) (void) printf(s,t)
	if (buf[0] == '?') {
	    PUTS("?                -- display this help message");
	    PUTS("fy fx ty tx      -- (4 numbers) display (fy,fx)->(ty,tx) move");
	    PUTS("s[croll] n t b m -- display scrolling sequence");
	    PUTF("r[eload]         -- reload terminal info for %s\n",
		 termname());
	    PUTS("l[oad] <term>    -- load terminal info for type <term>");
	    PUTS("d[elete] <cap>   -- delete named capability");
	    PUTS("i[nspect]        -- display terminal capabilities");
	    PUTS("c[ost]           -- dump cursor-optimization cost table");
	    PUTS("o[optimize]      -- toggle movement optimization");
	    PUTS("t[orture] <num>  -- torture-test with <num> random moves");
	    PUTS("q[uit]           -- quit the program");
	} else if (sscanf(buf, "%d %d %d %d", &fy, &fx, &ty, &tx) == 4) {
	    struct timeval before, after;

	    putchar('"');

	    gettimeofday(&before, NULL);
	    mvcur(fy, fx, ty, tx);
	    gettimeofday(&after, NULL);

	    printf("\" (%ld msec)\n",
		   (long) (after.tv_usec - before.tv_usec
			   + (after.tv_sec - before.tv_sec)
			   * 1000000));
	} else if (sscanf(buf, "s %d %d %d %d", &fy, &fx, &ty, &tx) == 4) {
	    struct timeval before, after;

	    putchar('"');

	    gettimeofday(&before, NULL);
	    _nc_scrolln(fy, fx, ty, tx);
	    gettimeofday(&after, NULL);

	    printf("\" (%ld msec)\n",
		   (long) (after.tv_usec - before.tv_usec + (after.tv_sec -
							     before.tv_sec)
			   * 1000000));
	} else if (buf[0] == 'r') {
	    _nc_STRCPY(tname, termname(), sizeof(tname));
	    load_term();
	} else if (sscanf(buf, "l %s", tname) == 1) {
	    load_term();
	} else if (sscanf(buf, "d %s", capname) == 1) {
	    struct name_table_entry const *np = _nc_find_entry(capname,
							       _nc_get_hash_table(FALSE));

	    if (np == NULL)
		(void) printf("No such capability as \"%s\"\n", capname);
	    else {
		switch (np->nte_type) {
		case BOOLEAN:
		    cur_term->type.Booleans[np->nte_index] = FALSE;
		    (void)
			printf("Boolean capability `%s' (%d) turned off.\n",
			       np->nte_name, np->nte_index);
		    break;

		case NUMBER:
		    cur_term->type.Numbers[np->nte_index] = ABSENT_NUMERIC;
		    (void) printf("Number capability `%s' (%d) set to -1.\n",
				  np->nte_name, np->nte_index);
		    break;

		case STRING:
		    cur_term->type.Strings[np->nte_index] = ABSENT_STRING;
		    (void) printf("String capability `%s' (%d) deleted.\n",
				  np->nte_name, np->nte_index);
		    break;
		}
	    }
	} else if (buf[0] == 'i') {
	    dump_init(NULL, F_TERMINFO, S_TERMINFO,
		      FALSE, 70, 0, 0, FALSE, FALSE, 0);
	    dump_entry(&TerminalType(cur_term), FALSE, TRUE, 0, 0);
	    putchar('\n');
	} else if (buf[0] == 'o') {
	    if (_nc_optimize_enable & OPTIMIZE_MVCUR) {
		_nc_optimize_enable &= ~OPTIMIZE_MVCUR;
		(void) puts("Optimization is now off.");
	    } else {
		_nc_optimize_enable |= OPTIMIZE_MVCUR;
		(void) puts("Optimization is now on.");
	    }
	}
	/*
	 * You can use the `t' test to profile and tune the movement
	 * optimizer.  Use iteration values in three digits or more.
	 * At above 5000 iterations the profile timing averages are stable
	 * to within a millisecond or three.
	 *
	 * The `overhead' field of the report will help you pick a
	 * COMPUTE_OVERHEAD figure appropriate for your processor and
	 * expected line speed.  The `total estimated time' is
	 * computation time plus a character-transmission time
	 * estimate computed from the number of transmits and the baud
	 * rate.
	 *
	 * Use this together with the `o' command to get a read on the
	 * optimizer's effectiveness.  Compare the total estimated times
	 * for `t' runs of the same length in both optimized and un-optimized
	 * modes.  As long as the optimized times are less, the optimizer
	 * is winning.
	 */
	else if (sscanf(buf, "t %d", &n) == 1) {
	    float cumtime = 0.0, perchar;
	    int speeds[] =
	    {2400, 9600, 14400, 19200, 28800, 38400, 0};

	    srand((unsigned) (getpid() + time((time_t *) 0)));
	    profiling = TRUE;
	    xmits = 0;
	    for (i = 0; i < n; i++) {
		/*
		 * This does a move test between two random locations,
		 * Random moves probably short-change the optimizer,
		 * which will work better on the short moves probably
		 * typical of doupdate()'s usage pattern.  Still,
		 * until we have better data...
		 */
#ifdef FIND_COREDUMP
		int from_y = roll(lines);
		int to_y = roll(lines);
		int from_x = roll(columns);
		int to_x = roll(columns);

		printf("(%d,%d) -> (%d,%d)\n", from_y, from_x, to_y, to_x);
		mvcur(from_y, from_x, to_y, to_x);
#else
		mvcur(roll(lines), roll(columns), roll(lines), roll(columns));
#endif /* FIND_COREDUMP */
		if (diff)
		    cumtime += diff;
	    }
	    profiling = FALSE;

	    /*
	     * Average milliseconds per character optimization time.
	     * This is the key figure to watch when tuning the optimizer.
	     */
	    perchar = cumtime / n;

	    (void) printf("%d moves (%ld chars) in %d msec, %f msec each:\n",
			  n, xmits, (int) cumtime, perchar);

	    for (i = 0; speeds[i]; i++) {
		/*
		 * Total estimated time for the moves, computation and
		 * transmission both. Transmission time is an estimate
		 * assuming 9 bits/char, 8 bits + 1 stop bit.
		 */
		float totalest = cumtime + xmits * 9 * 1e6 / speeds[i];

		/*
		 * Per-character optimization overhead in character transmits
		 * at the current speed.  Round this to the nearest integer
		 * to figure COMPUTE_OVERHEAD for the speed.
		 */
		float overhead = speeds[i] * perchar / 1e6;

		(void)
		    printf("%6d bps: %3.2f char-xmits overhead; total estimated time %15.2f\n",
			   speeds[i], overhead, totalest);
	    }
	} else if (buf[0] == 'c') {
	    (void) printf("char padding: %d\n", CURRENT_SCREEN->_char_padding);
	    (void) printf("cr cost: %d\n", CURRENT_SCREEN->_cr_cost);
	    (void) printf("cup cost: %d\n", CURRENT_SCREEN->_cup_cost);
	    (void) printf("home cost: %d\n", CURRENT_SCREEN->_home_cost);
	    (void) printf("ll cost: %d\n", CURRENT_SCREEN->_ll_cost);
#if USE_HARD_TABS
	    (void) printf("ht cost: %d\n", CURRENT_SCREEN->_ht_cost);
	    (void) printf("cbt cost: %d\n", CURRENT_SCREEN->_cbt_cost);
#endif /* USE_HARD_TABS */
	    (void) printf("cub1 cost: %d\n", CURRENT_SCREEN->_cub1_cost);
	    (void) printf("cuf1 cost: %d\n", CURRENT_SCREEN->_cuf1_cost);
	    (void) printf("cud1 cost: %d\n", CURRENT_SCREEN->_cud1_cost);
	    (void) printf("cuu1 cost: %d\n", CURRENT_SCREEN->_cuu1_cost);
	    (void) printf("cub cost: %d\n", CURRENT_SCREEN->_cub_cost);
	    (void) printf("cuf cost: %d\n", CURRENT_SCREEN->_cuf_cost);
	    (void) printf("cud cost: %d\n", CURRENT_SCREEN->_cud_cost);
	    (void) printf("cuu cost: %d\n", CURRENT_SCREEN->_cuu_cost);
	    (void) printf("hpa cost: %d\n", CURRENT_SCREEN->_hpa_cost);
	    (void) printf("vpa cost: %d\n", CURRENT_SCREEN->_vpa_cost);
	} else if (buf[0] == 'x' || buf[0] == 'q')
	    break;
	else
	    (void) puts("Invalid command.");
    }

    (void) fputs("rmcup:", stdout);
    _nc_mvcur_wrap();
    putchar('\n');

    return (0);
}