suggest_try_soundalike_prep(void)
{
    langp_T	*lp;
    int		lpi;
    slang_T	*slang;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    for (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	if (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != NULL)
	    // prepare the hashtable used by add_sound_suggest()
	    hash_init(&slang->sl_sounddone);
    }
}