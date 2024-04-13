qf_jump_print_msg(
	qf_info_T	*qi,
	int		qf_index,
	qfline_T	*qf_ptr,
	buf_T		*old_curbuf,
	linenr_T	old_lnum)
{
    linenr_T		i;
    int			len;

    // Update the screen before showing the message, unless the screen
    // scrolled up.
    if (!msg_scrolled)
	update_topline_redraw();
    sprintf((char *)IObuff, _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_count,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (char *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    len = (int)STRLEN(IObuff);
    qf_fmt_text(skipwhite(qf_ptr->qf_text), IObuff + len, IOSIZE - len);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = msg_scroll;
    if (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	msg_scroll = TRUE;
    else if (!msg_scrolled && shortmess(SHM_OVERALL))
	msg_scroll = FALSE;
    msg_attr_keep((char *)IObuff, 0, TRUE);
    msg_scroll = i;
}