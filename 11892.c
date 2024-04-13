print_ruler(int *tab_list)
{
    int last = 0;
    int stop;
    int n;

    /* first print a readable ruler */
    for (n = 0; n < max_cols; n += 10) {
	int ch = 1 + (n / 10);
	char buffer[20];
	_nc_SPRINTF(buffer, _nc_SLIMIT(sizeof(buffer))
		    "----+----%c",
		    ((ch < 10)
		     ? (ch + '0')
		     : (ch + 'A' - 10)));
	printf("%.*s", ((max_cols - n) > 10) ? 10 : (max_cols - n), buffer);
    }
    putchar('\n');

    /* now, print '*' for each stop */
    for (n = 0, last = 0; (tab_list[n] > 0) && (last < max_cols); ++n) {
	stop = tab_list[n];
	while (++last < stop) {
	    if (last <= max_cols) {
		putchar('-');
	    } else {
		break;
	    }
	}
	if (last <= max_cols) {
	    putchar('*');
	    last = stop;
	} else {
	    break;
	}
    }
    while (++last <= max_cols)
	putchar('-');
    putchar('\n');
}