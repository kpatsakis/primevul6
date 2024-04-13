find_keepcap_word(slang_T *slang, char_u *fword, char_u *kword)
{
    char_u	uword[MAXWLEN];		// "fword" in upper-case
    int		depth;
    idx_T	tryidx;

    // The following arrays are used at each depth in the tree.
    idx_T	arridx[MAXWLEN];
    int		round[MAXWLEN];
    int		fwordidx[MAXWLEN];
    int		uwordidx[MAXWLEN];
    int		kwordlen[MAXWLEN];

    int		flen, ulen;
    int		l;
    int		len;
    int		c;
    idx_T	lo, hi, m;
    char_u	*p;
    char_u	*byts = slang->sl_kbyts;    // array with bytes of the words
    idx_T	*idxs = slang->sl_kidxs;    // array with indexes

    if (byts == NULL)
    {
	// array is empty: "cannot happen"
	*kword = NUL;
	return;
    }

    // Make an all-cap version of "fword".
    allcap_copy(fword, uword);

    // Each character needs to be tried both case-folded and upper-case.
    // All this gets very complicated if we keep in mind that changing case
    // may change the byte length of a multi-byte character...
    depth = 0;
    arridx[0] = 0;
    round[0] = 0;
    fwordidx[0] = 0;
    uwordidx[0] = 0;
    kwordlen[0] = 0;
    while (depth >= 0)
    {
	if (fword[fwordidx[depth]] == NUL)
	{
	    // We are at the end of "fword".  If the tree allows a word to end
	    // here we have found a match.
	    if (byts[arridx[depth] + 1] == 0)
	    {
		kword[kwordlen[depth]] = NUL;
		return;
	    }

	    // kword is getting too long, continue one level up
	    --depth;
	}
	else if (++round[depth] > 2)
	{
	    // tried both fold-case and upper-case character, continue one
	    // level up
	    --depth;
	}
	else
	{
	    // round[depth] == 1: Try using the folded-case character.
	    // round[depth] == 2: Try using the upper-case character.
	    if (has_mbyte)
	    {
		flen = MB_CPTR2LEN(fword + fwordidx[depth]);
		ulen = MB_CPTR2LEN(uword + uwordidx[depth]);
	    }
	    else
		ulen = flen = 1;
	    if (round[depth] == 1)
	    {
		p = fword + fwordidx[depth];
		l = flen;
	    }
	    else
	    {
		p = uword + uwordidx[depth];
		l = ulen;
	    }

	    for (tryidx = arridx[depth]; l > 0; --l)
	    {
		// Perform a binary search in the list of accepted bytes.
		len = byts[tryidx++];
		c = *p++;
		lo = tryidx;
		hi = tryidx + len - 1;
		while (lo < hi)
		{
		    m = (lo + hi) / 2;
		    if (byts[m] > c)
			hi = m - 1;
		    else if (byts[m] < c)
			lo = m + 1;
		    else
		    {
			lo = hi = m;
			break;
		    }
		}

		// Stop if there is no matching byte.
		if (hi < lo || byts[lo] != c)
		    break;

		// Continue at the child (if there is one).
		tryidx = idxs[lo];
	    }

	    if (l == 0)
	    {
		// Found the matching char.  Copy it to "kword" and go a
		// level deeper.
		if (round[depth] == 1)
		{
		    STRNCPY(kword + kwordlen[depth], fword + fwordidx[depth],
									flen);
		    kwordlen[depth + 1] = kwordlen[depth] + flen;
		}
		else
		{
		    STRNCPY(kword + kwordlen[depth], uword + uwordidx[depth],
									ulen);
		    kwordlen[depth + 1] = kwordlen[depth] + ulen;
		}
		fwordidx[depth + 1] = fwordidx[depth] + flen;
		uwordidx[depth + 1] = uwordidx[depth] + ulen;

		++depth;
		arridx[depth] = tryidx;
		round[depth] = 0;
	    }
	}
    }

    // Didn't find it: "cannot happen".
    *kword = NUL;
}