add_sound_suggest(
    suginfo_T	*su,
    char_u	*goodword,
    int		score,		// soundfold score
    langp_T	*lp)
{
    slang_T	*slang = lp->lp_slang;	// language for sound folding
    int		sfwordnr;
    char_u	*nrline;
    int		orgnr;
    char_u	theword[MAXWLEN];
    int		i;
    int		wlen;
    char_u	*byts;
    idx_T	*idxs;
    int		n;
    int		wordcount;
    int		wc;
    int		goodscore;
    hash_T	hash;
    hashitem_T  *hi;
    sftword_T	*sft;
    int		bc, gc;
    int		limit;

    // It's very well possible that the same soundfold word is found several
    // times with different scores.  Since the following is quite slow only do
    // the words that have a better score than before.  Use a hashtable to
    // remember the words that have been done.
    hash = hash_hash(goodword);
    hi = hash_lookup(&slang->sl_sounddone, goodword, hash);
    if (HASHITEM_EMPTY(hi))
    {
	sft = alloc(sizeof(sftword_T) + STRLEN(goodword));
	if (sft != NULL)
	{
	    sft->sft_score = score;
	    STRCPY(sft->sft_word, goodword);
	    hash_add_item(&slang->sl_sounddone, hi, sft->sft_word, hash);
	}
    }
    else
    {
	sft = HI2SFT(hi);
	if (score >= sft->sft_score)
	    return;
	sft->sft_score = score;
    }

    // Find the word nr in the soundfold tree.
    sfwordnr = soundfold_find(slang, goodword);
    if (sfwordnr < 0)
    {
	internal_error("add_sound_suggest()");
	return;
    }

    // go over the list of good words that produce this soundfold word
    nrline = ml_get_buf(slang->sl_sugbuf, (linenr_T)(sfwordnr + 1), FALSE);
    orgnr = 0;
    while (*nrline != NUL)
    {
	// The wordnr was stored in a minimal nr of bytes as an offset to the
	// previous wordnr.
	orgnr += bytes2offset(&nrline);

	byts = slang->sl_fbyts;
	idxs = slang->sl_fidxs;

	// Lookup the word "orgnr" one of the two tries.
	n = 0;
	wordcount = 0;
	for (wlen = 0; wlen < MAXWLEN - 3; ++wlen)
	{
	    i = 1;
	    if (wordcount == orgnr && byts[n + 1] == NUL)
		break;	// found end of word

	    if (byts[n + 1] == NUL)
		++wordcount;

	    // skip over the NUL bytes
	    for ( ; byts[n + i] == NUL; ++i)
		if (i > byts[n])	// safety check
		{
		    STRCPY(theword + wlen, "BAD");
		    wlen += 3;
		    goto badword;
		}

	    // One of the siblings must have the word.
	    for ( ; i < byts[n]; ++i)
	    {
		wc = idxs[idxs[n + i]];	// nr of words under this byte
		if (wordcount + wc > orgnr)
		    break;
		wordcount += wc;
	    }

	    theword[wlen] = byts[n + i];
	    n = idxs[n + i];
	}
badword:
	theword[wlen] = NUL;

	// Go over the possible flags and regions.
	for (; i <= byts[n] && byts[n + i] == NUL; ++i)
	{
	    char_u	cword[MAXWLEN];
	    char_u	*p;
	    int		flags = (int)idxs[n + i];

	    // Skip words with the NOSUGGEST flag
	    if (flags & WF_NOSUGGEST)
		continue;

	    if (flags & WF_KEEPCAP)
	    {
		// Must find the word in the keep-case tree.
		find_keepcap_word(slang, theword, cword);
		p = cword;
	    }
	    else
	    {
		flags |= su->su_badflags;
		if ((flags & WF_CAPMASK) != 0)
		{
		    // Need to fix case according to "flags".
		    make_case_word(theword, cword, flags);
		    p = cword;
		}
		else
		    p = theword;
	    }

	    // Add the suggestion.
	    if (sps_flags & SPS_DOUBLE)
	    {
		// Add the suggestion if the score isn't too bad.
		if (score <= su->su_maxscore)
		    add_suggestion(su, &su->su_sga, p, su->su_badlen,
					       score, 0, FALSE, slang, FALSE);
	    }
	    else
	    {
		// Add a penalty for words in another region.
		if ((flags & WF_REGION)
			    && (((unsigned)flags >> 16) & lp->lp_region) == 0)
		    goodscore = SCORE_REGION;
		else
		    goodscore = 0;

		// Add a small penalty for changing the first letter from
		// lower to upper case.  Helps for "tath" -> "Kath", which is
		// less common than "tath" -> "path".  Don't do it when the
		// letter is the same, that has already been counted.
		gc = PTR2CHAR(p);
		if (SPELL_ISUPPER(gc))
		{
		    bc = PTR2CHAR(su->su_badword);
		    if (!SPELL_ISUPPER(bc)
				      && SPELL_TOFOLD(bc) != SPELL_TOFOLD(gc))
			goodscore += SCORE_ICASE / 2;
		}

		// Compute the score for the good word.  This only does letter
		// insert/delete/swap/replace.  REP items are not considered,
		// which may make the score a bit higher.
		// Use a limit for the score to make it work faster.  Use
		// MAXSCORE(), because RESCORE() will change the score.
		// If the limit is very high then the iterative method is
		// inefficient, using an array is quicker.
		limit = MAXSCORE(su->su_sfmaxscore - goodscore, score);
		if (limit > SCORE_LIMITMAX)
		    goodscore += spell_edit_score(slang, su->su_badword, p);
		else
		    goodscore += spell_edit_score_limit(slang, su->su_badword,
								    p, limit);

		// When going over the limit don't bother to do the rest.
		if (goodscore < SCORE_MAXMAX)
		{
		    // Give a bonus to words seen before.
		    goodscore = score_wordcount_adj(slang, goodscore, p, FALSE);

		    // Add the suggestion if the score isn't too bad.
		    goodscore = RESCORE(goodscore, score);
		    if (goodscore <= su->su_sfmaxscore)
			add_suggestion(su, &su->su_ga, p, su->su_badlen,
					 goodscore, score, TRUE, slang, TRUE);
		}
	    }
	}
	// smsg("word %s (%d): %s (%d)", sftword, sftnr, theword, orgnr);
    }
}