score_wordcount_adj(
    slang_T	*slang,
    int		score,
    char_u	*word,
    int		split)	    // word was split, less bonus
{
    hashitem_T	*hi;
    wordcount_T	*wc;
    int		bonus;
    int		newscore;

    hi = hash_find(&slang->sl_wordcount, word);
    if (!HASHITEM_EMPTY(hi))
    {
	wc = HI2WC(hi);
	if (wc->wc_count < SCORE_THRES2)
	    bonus = SCORE_COMMON1;
	else if (wc->wc_count < SCORE_THRES3)
	    bonus = SCORE_COMMON2;
	else
	    bonus = SCORE_COMMON3;
	if (split)
	    newscore = score - bonus / 2;
	else
	    newscore = score - bonus;
	if (newscore < 0)
	    return 0;
	return newscore;
    }
    return score;
}