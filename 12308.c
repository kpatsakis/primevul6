find_ident_under_cursor(char_u **text, int find_type)
{
    return find_ident_at_pos(curwin, curwin->w_cursor.lnum,
				curwin->w_cursor.col, text, NULL, find_type);
}