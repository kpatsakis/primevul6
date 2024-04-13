ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    if ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	return;

    win = qf_find_win(qi);
    if (win != NULL && win->w_cursor.lnum != win->w_buffer->b_ml.ml_line_count)
	qf_win_goto(win, win->w_buffer->b_ml.ml_line_count);
}