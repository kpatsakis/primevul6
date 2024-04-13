add_suggestion(
    suginfo_T	*su,
    garray_T	*gap,		// either su_ga or su_sga
    char_u	*goodword,
    int		badlenarg,	// len of bad word replaced with "goodword"
    int		score,
    int		altscore,
    int		had_bonus,	// value for st_had_bonus
    slang_T	*slang,		// language for sound folding
    int		maxsf)		// su_maxscore applies to soundfold score,
				// su_sfmaxscore to the total score.
{
    int		goodlen;	// len of goodword changed
    int		badlen;		// len of bad word changed
    suggest_T   *stp;
    suggest_T   new_sug;
    int		i;
    char_u	*pgood, *pbad;

    // Minimize "badlen" for consistency.  Avoids that changing "the the" to
    // "thee the" is added next to changing the first "the" the "thee".
    pgood = goodword + STRLEN(goodword);
    pbad = su->su_badptr + badlenarg;
    for (;;)
    {
	goodlen = (int)(pgood - goodword);
	badlen = (int)(pbad - su->su_badptr);
	if (goodlen <= 0 || badlen <= 0)
	    break;
	MB_PTR_BACK(goodword, pgood);
	MB_PTR_BACK(su->su_badptr, pbad);
	if (has_mbyte)
	{
	    if (mb_ptr2char(pgood) != mb_ptr2char(pbad))
		break;
	}
	else if (*pgood != *pbad)
		break;
    }

    if (badlen == 0 && goodlen == 0)
	// goodword doesn't change anything; may happen for "the the" changing
	// the first "the" to itself.
	return;

    if (gap->ga_len == 0)
	i = -1;
    else
    {
	// Check if the word is already there.  Also check the length that is
	// being replaced "thes," -> "these" is a different suggestion from
	// "thes" -> "these".
	stp = &SUG(*gap, 0);
	for (i = gap->ga_len; --i >= 0; ++stp)
	    if (stp->st_wordlen == goodlen
		    && stp->st_orglen == badlen
		    && STRNCMP(stp->st_word, goodword, goodlen) == 0)
	    {
		// Found it.  Remember the word with the lowest score.
		if (stp->st_slang == NULL)
		    stp->st_slang = slang;

		new_sug.st_score = score;
		new_sug.st_altscore = altscore;
		new_sug.st_had_bonus = had_bonus;

		if (stp->st_had_bonus != had_bonus)
		{
		    // Only one of the two had the soundalike score computed.
		    // Need to do that for the other one now, otherwise the
		    // scores can't be compared.  This happens because
		    // suggest_try_change() doesn't compute the soundalike
		    // word to keep it fast, while some special methods set
		    // the soundalike score to zero.
		    if (had_bonus)
			rescore_one(su, stp);
		    else
		    {
			new_sug.st_word = stp->st_word;
			new_sug.st_wordlen = stp->st_wordlen;
			new_sug.st_slang = stp->st_slang;
			new_sug.st_orglen = badlen;
			rescore_one(su, &new_sug);
		    }
		}

		if (stp->st_score > new_sug.st_score)
		{
		    stp->st_score = new_sug.st_score;
		    stp->st_altscore = new_sug.st_altscore;
		    stp->st_had_bonus = new_sug.st_had_bonus;
		}
		break;
	    }
    }

    if (i < 0 && ga_grow(gap, 1) == OK)
    {
	// Add a suggestion.
	stp = &SUG(*gap, gap->ga_len);
	stp->st_word = vim_strnsave(goodword, goodlen);
	if (stp->st_word != NULL)
	{
	    stp->st_wordlen = goodlen;
	    stp->st_score = score;
	    stp->st_altscore = altscore;
	    stp->st_had_bonus = had_bonus;
	    stp->st_orglen = badlen;
	    stp->st_slang = slang;
	    ++gap->ga_len;

	    // If we have too many suggestions now, sort the list and keep
	    // the best suggestions.
	    if (gap->ga_len > SUG_MAX_COUNT(su))
	    {
		if (maxsf)
		    su->su_sfmaxscore = cleanup_suggestions(gap,
				      su->su_sfmaxscore, SUG_CLEAN_COUNT(su));
		else
		    su->su_maxscore = cleanup_suggestions(gap,
					su->su_maxscore, SUG_CLEAN_COUNT(su));
	    }
	}
    }
}