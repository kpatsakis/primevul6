score_comp_sal(suginfo_T *su)
{
    langp_T	*lp;
    char_u	badsound[MAXWLEN];
    int		i;
    suggest_T   *stp;
    suggest_T   *sstp;
    int		score;
    int		lpi;

    if (ga_grow(&su->su_sga, su->su_ga.ga_len) == FAIL)
	return;

    // Use the sound-folding of the first language that supports it.
    for (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	if (lp->lp_slang->sl_sal.ga_len > 0)
	{
	    // soundfold the bad word
	    spell_soundfold(lp->lp_slang, su->su_fbadword, TRUE, badsound);

	    for (i = 0; i < su->su_ga.ga_len; ++i)
	    {
		stp = &SUG(su->su_ga, i);

		// Case-fold the suggested word, sound-fold it and compute the
		// sound-a-like score.
		score = stp_sal_score(stp, su, lp->lp_slang, badsound);
		if (score < SCORE_MAXMAX)
		{
		    // Add the suggestion.
		    sstp = &SUG(su->su_sga, su->su_sga.ga_len);
		    sstp->st_word = vim_strsave(stp->st_word);
		    if (sstp->st_word != NULL)
		    {
			sstp->st_wordlen = stp->st_wordlen;
			sstp->st_score = score;
			sstp->st_altscore = 0;
			sstp->st_orglen = stp->st_orglen;
			++su->su_sga.ga_len;
		    }
		}
	    }
	    break;
	}
    }
}