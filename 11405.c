check_suggestions(
    suginfo_T	*su,
    garray_T	*gap)		    // either su_ga or su_sga
{
    suggest_T   *stp;
    int		i;
    char_u	longword[MAXWLEN + 1];
    int		len;
    hlf_T	attr;

    if (gap->ga_len == 0)
	return;
    stp = &SUG(*gap, 0);
    for (i = gap->ga_len - 1; i >= 0; --i)
    {
	// Need to append what follows to check for "the the".
	vim_strncpy(longword, stp[i].st_word, MAXWLEN);
	len = stp[i].st_wordlen;
	vim_strncpy(longword + len, su->su_badptr + stp[i].st_orglen,
							       MAXWLEN - len);
	attr = HLF_COUNT;
	(void)spell_check(curwin, longword, &attr, NULL, FALSE);
	if (attr != HLF_COUNT)
	{
	    // Remove this entry.
	    vim_free(stp[i].st_word);
	    --gap->ga_len;
	    if (i < gap->ga_len)
		mch_memmove(stp + i, stp + i + 1,
				       sizeof(suggest_T) * (gap->ga_len - i));
	}
    }
}