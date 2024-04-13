ex_messages(exarg_T *eap)
{
    struct msg_hist *p;
    char_u	    *s;
    int		    c = 0;

    if (STRCMP(eap->arg, "clear") == 0)
    {
	int keep = eap->addr_count == 0 ? 0 : eap->line2;

	while (msg_hist_len > keep)
	    (void)delete_first_msg();
	return;
    }

    if (*eap->arg != NUL)
    {
	emsg(_(e_invalid_argument));
	return;
    }

    msg_hist_off = TRUE;

    p = first_msg_hist;
    if (eap->addr_count != 0)
    {
	// Count total messages
	for (; p != NULL && !got_int; p = p->next)
	    c++;

	c -= eap->line2;

	// Skip without number of messages specified
	for (p = first_msg_hist; p != NULL && !got_int && c > 0;
						    p = p->next, c--);
    }

    if (p == first_msg_hist)
    {
#ifdef FEAT_MULTI_LANG
	s = get_mess_lang();
#else
	s = mch_getenv((char_u *)"LANG");
#endif
	if (s != NULL && *s != NUL)
	    // The next comment is extracted by xgettext and put in po file for
	    // translators to read.
	    msg_attr(
		    // Translator: Please replace the name and email address
		    // with the appropriate text for your translation.
		    _("Messages maintainer: Bram Moolenaar <Bram@vim.org>"),
		    HL_ATTR(HLF_T));
    }

    // Display what was not skipped.
    for (; p != NULL && !got_int; p = p->next)
	if (p->msg != NULL)
	    msg_attr((char *)p->msg, p->attr);

    msg_hist_off = FALSE;
}