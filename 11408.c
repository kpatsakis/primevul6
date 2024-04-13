soundalike_score(
    char_u	*goodstart,	// sound-folded good word
    char_u	*badstart)	// sound-folded bad word
{
    char_u	*goodsound = goodstart;
    char_u	*badsound = badstart;
    int		goodlen;
    int		badlen;
    int		n;
    char_u	*pl, *ps;
    char_u	*pl2, *ps2;
    int		score = 0;

    // Adding/inserting "*" at the start (word starts with vowel) shouldn't be
    // counted so much, vowels halfway the word aren't counted at all.
    if ((*badsound == '*' || *goodsound == '*') && *badsound != *goodsound)
    {
	if ((badsound[0] == NUL && goodsound[1] == NUL)
	    || (goodsound[0] == NUL && badsound[1] == NUL))
	    // changing word with vowel to word without a sound
	    return SCORE_DEL;
	if (badsound[0] == NUL || goodsound[0] == NUL)
	    // more than two changes
	    return SCORE_MAXMAX;

	if (badsound[1] == goodsound[1]
		|| (badsound[1] != NUL
		    && goodsound[1] != NUL
		    && badsound[2] == goodsound[2]))
	{
	    // handle like a substitute
	}
	else
	{
	    score = 2 * SCORE_DEL / 3;
	    if (*badsound == '*')
		++badsound;
	    else
		++goodsound;
	}
    }

    goodlen = (int)STRLEN(goodsound);
    badlen = (int)STRLEN(badsound);

    // Return quickly if the lengths are too different to be fixed by two
    // changes.
    n = goodlen - badlen;
    if (n < -2 || n > 2)
	return SCORE_MAXMAX;

    if (n > 0)
    {
	pl = goodsound;	    // goodsound is longest
	ps = badsound;
    }
    else
    {
	pl = badsound;	    // badsound is longest
	ps = goodsound;
    }

    // Skip over the identical part.
    while (*pl == *ps && *pl != NUL)
    {
	++pl;
	++ps;
    }

    switch (n)
    {
	case -2:
	case 2:
	    // Must delete two characters from "pl".
	    ++pl;	// first delete
	    while (*pl == *ps)
	    {
		++pl;
		++ps;
	    }
	    // strings must be equal after second delete
	    if (STRCMP(pl + 1, ps) == 0)
		return score + SCORE_DEL * 2;

	    // Failed to compare.
	    break;

	case -1:
	case 1:
	    // Minimal one delete from "pl" required.

	    // 1: delete
	    pl2 = pl + 1;
	    ps2 = ps;
	    while (*pl2 == *ps2)
	    {
		if (*pl2 == NUL)	// reached the end
		    return score + SCORE_DEL;
		++pl2;
		++ps2;
	    }

	    // 2: delete then swap, then rest must be equal
	    if (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		return score + SCORE_DEL + SCORE_SWAP;

	    // 3: delete then substitute, then the rest must be equal
	    if (STRCMP(pl2 + 1, ps2 + 1) == 0)
		return score + SCORE_DEL + SCORE_SUBST;

	    // 4: first swap then delete
	    if (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		while (*pl2 == *ps2)
		{
		    ++pl2;
		    ++ps2;
		}
		// delete a char and then strings must be equal
		if (STRCMP(pl2 + 1, ps2) == 0)
		    return score + SCORE_SWAP + SCORE_DEL;
	    }

	    // 5: first substitute then delete
	    pl2 = pl + 1;	    // substitute, skip one char
	    ps2 = ps + 1;
	    while (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    // delete a char and then strings must be equal
	    if (STRCMP(pl2 + 1, ps2) == 0)
		return score + SCORE_SUBST + SCORE_DEL;

	    // Failed to compare.
	    break;

	case 0:
	    // Lengths are equal, thus changes must result in same length: An
	    // insert is only possible in combination with a delete.
	    // 1: check if for identical strings
	    if (*pl == NUL)
		return score;

	    // 2: swap
	    if (pl[0] == ps[1] && pl[1] == ps[0])
	    {
		pl2 = pl + 2;	    // swap, skip two chars
		ps2 = ps + 2;
		while (*pl2 == *ps2)
		{
		    if (*pl2 == NUL)	// reached the end
			return score + SCORE_SWAP;
		    ++pl2;
		    ++ps2;
		}
		// 3: swap and swap again
		if (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		    return score + SCORE_SWAP + SCORE_SWAP;

		// 4: swap and substitute
		if (STRCMP(pl2 + 1, ps2 + 1) == 0)
		    return score + SCORE_SWAP + SCORE_SUBST;
	    }

	    // 5: substitute
	    pl2 = pl + 1;
	    ps2 = ps + 1;
	    while (*pl2 == *ps2)
	    {
		if (*pl2 == NUL)	// reached the end
		    return score + SCORE_SUBST;
		++pl2;
		++ps2;
	    }

	    // 6: substitute and swap
	    if (pl2[0] == ps2[1] && pl2[1] == ps2[0]
					     && STRCMP(pl2 + 2, ps2 + 2) == 0)
		return score + SCORE_SUBST + SCORE_SWAP;

	    // 7: substitute and substitute
	    if (STRCMP(pl2 + 1, ps2 + 1) == 0)
		return score + SCORE_SUBST + SCORE_SUBST;

	    // 8: insert then delete
	    pl2 = pl;
	    ps2 = ps + 1;
	    while (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    if (STRCMP(pl2 + 1, ps2) == 0)
		return score + SCORE_INS + SCORE_DEL;

	    // 9: delete then insert
	    pl2 = pl + 1;
	    ps2 = ps;
	    while (*pl2 == *ps2)
	    {
		++pl2;
		++ps2;
	    }
	    if (STRCMP(pl2, ps2 + 1) == 0)
		return score + SCORE_INS + SCORE_DEL;

	    // Failed to compare.
	    break;
    }

    return SCORE_MAXMAX;
}