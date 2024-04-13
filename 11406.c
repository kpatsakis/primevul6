spell_edit_score(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword)
{
    int		*cnt;
    int		badlen, goodlen;	// lengths including NUL
    int		j, i;
    int		t;
    int		bc, gc;
    int		pbc, pgc;
    char_u	*p;
    int		wbadword[MAXWLEN];
    int		wgoodword[MAXWLEN];

    if (has_mbyte)
    {
	// Get the characters from the multi-byte strings and put them in an
	// int array for easy access.
	for (p = badword, badlen = 0; *p != NUL; )
	    wbadword[badlen++] = mb_cptr2char_adv(&p);
	wbadword[badlen++] = 0;
	for (p = goodword, goodlen = 0; *p != NUL; )
	    wgoodword[goodlen++] = mb_cptr2char_adv(&p);
	wgoodword[goodlen++] = 0;
    }
    else
    {
	badlen = (int)STRLEN(badword) + 1;
	goodlen = (int)STRLEN(goodword) + 1;
    }

    // We use "cnt" as an array: CNT(badword_idx, goodword_idx).
#define CNT(a, b)   cnt[(a) + (b) * (badlen + 1)]
    cnt = ALLOC_MULT(int, (badlen + 1) * (goodlen + 1));
    if (cnt == NULL)
	return 0;	// out of memory

    CNT(0, 0) = 0;
    for (j = 1; j <= goodlen; ++j)
	CNT(0, j) = CNT(0, j - 1) + SCORE_INS;

    for (i = 1; i <= badlen; ++i)
    {
	CNT(i, 0) = CNT(i - 1, 0) + SCORE_DEL;
	for (j = 1; j <= goodlen; ++j)
	{
	    if (has_mbyte)
	    {
		bc = wbadword[i - 1];
		gc = wgoodword[j - 1];
	    }
	    else
	    {
		bc = badword[i - 1];
		gc = goodword[j - 1];
	    }
	    if (bc == gc)
		CNT(i, j) = CNT(i - 1, j - 1);
	    else
	    {
		// Use a better score when there is only a case difference.
		if (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		    CNT(i, j) = SCORE_ICASE + CNT(i - 1, j - 1);
		else
		{
		    // For a similar character use SCORE_SIMILAR.
		    if (slang != NULL
			    && slang->sl_has_map
			    && similar_chars(slang, gc, bc))
			CNT(i, j) = SCORE_SIMILAR + CNT(i - 1, j - 1);
		    else
			CNT(i, j) = SCORE_SUBST + CNT(i - 1, j - 1);
		}

		if (i > 1 && j > 1)
		{
		    if (has_mbyte)
		    {
			pbc = wbadword[i - 2];
			pgc = wgoodword[j - 2];
		    }
		    else
		    {
			pbc = badword[i - 2];
			pgc = goodword[j - 2];
		    }
		    if (bc == pgc && pbc == gc)
		    {
			t = SCORE_SWAP + CNT(i - 2, j - 2);
			if (t < CNT(i, j))
			    CNT(i, j) = t;
		    }
		}
		t = SCORE_DEL + CNT(i - 1, j);
		if (t < CNT(i, j))
		    CNT(i, j) = t;
		t = SCORE_INS + CNT(i, j - 1);
		if (t < CNT(i, j))
		    CNT(i, j) = t;
	    }
	}
    }

    i = CNT(badlen - 1, goodlen - 1);
    vim_free(cnt);
    return i;
}