spell_suggest_list(
    garray_T	*gap,
    char_u	*word,
    int		maxcount,	// maximum nr of suggestions
    int		need_cap,	// 'spellcapcheck' matched
    int		interactive)
{
    suginfo_T	sug;
    int		i;
    suggest_T	*stp;
    char_u	*wcopy;

    spell_find_suggest(word, 0, &sug, maxcount, FALSE, need_cap, interactive);

    // Make room in "gap".
    ga_init2(gap, sizeof(char_u *), sug.su_ga.ga_len + 1);
    if (ga_grow(gap, sug.su_ga.ga_len) == OK)
    {
	for (i = 0; i < sug.su_ga.ga_len; ++i)
	{
	    stp = &SUG(sug.su_ga, i);

	    // The suggested word may replace only part of "word", add the not
	    // replaced part.
	    wcopy = alloc(stp->st_wordlen
		      + (unsigned)STRLEN(sug.su_badptr + stp->st_orglen) + 1);
	    if (wcopy == NULL)
		break;
	    STRCPY(wcopy, stp->st_word);
	    STRCPY(wcopy + stp->st_wordlen, sug.su_badptr + stp->st_orglen);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = wcopy;
	}
    }

    spell_find_cleanup(&sug);
}