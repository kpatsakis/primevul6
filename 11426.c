suggest_try_soundalike_finish(void)
{
    langp_T	*lp;
    int		lpi;
    slang_T	*slang;
    int		todo;
    hashitem_T	*hi;

    // Do this for all languages that support sound folding and for which a
    // .sug file has been loaded.
    for (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	if (slang->sl_sal.ga_len > 0 && slang->sl_sbyts != NULL)
	{
	    // Free the info about handled words.
	    todo = (int)slang->sl_sounddone.ht_used;
	    for (hi = slang->sl_sounddone.ht_array; todo > 0; ++hi)
		if (!HASHITEM_EMPTY(hi))
		{
		    vim_free(HI2SFT(hi));
		    --todo;
		}

	    // Clear the hashtable, it may also be used by another region.
	    hash_clear(&slang->sl_sounddone);
	    hash_init(&slang->sl_sounddone);
	}
    }
}