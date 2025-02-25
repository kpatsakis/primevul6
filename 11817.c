check_cursor(TERMTYPE2 *tp)
{
    int count;
    char *list[4];

    if (hard_copy) {
	check_noaddress(tp, "hard_copy");
    } else if (generic_type) {
	check_noaddress(tp, "generic_type");
    } else if (strchr(tp->term_names, '+') == 0) {
	int y = 0;
	int x = 0;
	if (PRESENT(column_address))
	    ++y;
	if (PRESENT(cursor_address))
	    y = x = 10;
	if (PRESENT(cursor_home))
	    ++y, ++x;
	if (PRESENT(cursor_mem_address))
	    y = x = 10;
	if (PRESENT(cursor_to_ll))
	    ++y, ++x;
	if (PRESENT(row_address))
	    ++x;
	if (PRESENT(cursor_down))
	    ++y;
	if (PRESENT(cursor_up))
	    ++y;
	if (PRESENT(cursor_left))
	    ++x;
	if (PRESENT(cursor_right))
	    ++x;
	if (x < 2 && y < 2) {
	    _nc_warning("terminal lacks cursor addressing");
	} else {
	    if (x < 2)
		_nc_warning("terminal lacks cursor column-addressing");
	    if (y < 2)
		_nc_warning("terminal lacks cursor row-addressing");
	}
    }

    /* it is rare to have an insert-line feature without a matching delete */
    ANDMISSING(parm_insert_line, insert_line);
    ANDMISSING(parm_delete_line, delete_line);
    ANDMISSING(parm_insert_line, parm_delete_line);

    /* if we have a parameterized form, then the non-parameterized is easy */
    ANDMISSING(parm_down_cursor, cursor_down);
    ANDMISSING(parm_up_cursor, cursor_up);
    ANDMISSING(parm_left_cursor, cursor_left);
    ANDMISSING(parm_right_cursor, cursor_right);

    /* Given any of a set of cursor movement, the whole set should be present.
     * Technically this is not true (we could use cursor_address to fill in
     * unsupported controls), but it is likely.
     */
    count = 0;
    if (PRESENT(parm_down_cursor)) {
	list[count++] = parm_down_cursor;
    }
    if (PRESENT(parm_up_cursor)) {
	list[count++] = parm_up_cursor;
    }
    if (PRESENT(parm_left_cursor)) {
	list[count++] = parm_left_cursor;
    }
    if (PRESENT(parm_right_cursor)) {
	list[count++] = parm_right_cursor;
    }
    if (count == 4) {
	check_ansi_cursor(list);
    } else if (count != 0) {
	EXPECTED(parm_down_cursor);
	EXPECTED(parm_up_cursor);
	EXPECTED(parm_left_cursor);
	EXPECTED(parm_right_cursor);
    }

    count = 0;
    if (PRESENT(cursor_down)) {
	list[count++] = cursor_down;
    }
    if (PRESENT(cursor_up)) {
	list[count++] = cursor_up;
    }
    if (PRESENT(cursor_left)) {
	list[count++] = cursor_left;
    }
    if (PRESENT(cursor_right)) {
	list[count++] = cursor_right;
    }
    if (count == 4) {
	check_ansi_cursor(list);
    } else if (count != 0) {
	count = 0;
	if (PRESENT(cursor_down) && strcmp(cursor_down, "\n"))
	    ++count;
	if (PRESENT(cursor_left) && strcmp(cursor_left, "\b"))
	    ++count;
	if (PRESENT(cursor_up) && strlen(cursor_up) > 1)
	    ++count;
	if (PRESENT(cursor_right) && strlen(cursor_right) > 1)
	    ++count;
	if (count) {
	    EXPECTED(cursor_down);
	    EXPECTED(cursor_up);
	    EXPECTED(cursor_left);
	    EXPECTED(cursor_right);
	}
    }
}