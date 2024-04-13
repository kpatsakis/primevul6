suggest_try_change(suginfo_T *su)
{
    char_u	fword[MAXWLEN];	    // copy of the bad word, case-folded
    int		n;
    char_u	*p;
    int		lpi;
    langp_T	*lp;

    // We make a copy of the case-folded bad word, so that we can modify it
    // to find matches (esp. REP items).  Append some more text, changing
    // chars after the bad word may help.
    STRCPY(fword, su->su_fbadword);
    n = (int)STRLEN(fword);
    p = su->su_badptr + su->su_badlen;
    (void)spell_casefold(curwin, p, (int)STRLEN(p), fword + n, MAXWLEN - n);

    // Make sure the resulting text is not longer than the original text.
    n = (int)STRLEN(su->su_badptr);
    if (n < MAXWLEN)
	fword[n] = NUL;

    for (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);

	// If reloading a spell file fails it's still in the list but
	// everything has been cleared.
	if (lp->lp_slang->sl_fbyts == NULL)
	    continue;

	// Try it for this language.  Will add possible suggestions.
#ifdef SUGGEST_PROFILE
	prof_init();
#endif
	suggest_trie_walk(su, lp, fword, FALSE);
#ifdef SUGGEST_PROFILE
	prof_report("try_change");
#endif
    }
}