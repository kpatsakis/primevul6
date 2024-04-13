stp_sal_score(
    suggest_T	*stp,
    suginfo_T	*su,
    slang_T	*slang,
    char_u	*badsound)	// sound-folded badword
{
    char_u	*p;
    char_u	*pbad;
    char_u	*pgood;
    char_u	badsound2[MAXWLEN];
    char_u	fword[MAXWLEN];
    char_u	goodsound[MAXWLEN];
    char_u	goodword[MAXWLEN];
    int		lendiff;

    lendiff = (int)(su->su_badlen - stp->st_orglen);
    if (lendiff >= 0)
	pbad = badsound;
    else
    {
	// soundfold the bad word with more characters following
	(void)spell_casefold(curwin,
				su->su_badptr, stp->st_orglen, fword, MAXWLEN);

	// When joining two words the sound often changes a lot.  E.g., "t he"
	// sounds like "t h" while "the" sounds like "@".  Avoid that by
	// removing the space.  Don't do it when the good word also contains a
	// space.
	if (VIM_ISWHITE(su->su_badptr[su->su_badlen])
					 && *skiptowhite(stp->st_word) == NUL)
	    for (p = fword; *(p = skiptowhite(p)) != NUL; )
		STRMOVE(p, p + 1);

	spell_soundfold(slang, fword, TRUE, badsound2);
	pbad = badsound2;
    }

    if (lendiff > 0 && stp->st_wordlen + lendiff < MAXWLEN)
    {
	// Add part of the bad word to the good word, so that we soundfold
	// what replaces the bad word.
	STRCPY(goodword, stp->st_word);
	vim_strncpy(goodword + stp->st_wordlen,
			    su->su_badptr + su->su_badlen - lendiff, lendiff);
	pgood = goodword;
    }
    else
	pgood = stp->st_word;

    // Sound-fold the word and compute the score for the difference.
    spell_soundfold(slang, pgood, FALSE, goodsound);

    return soundalike_score(goodsound, pbad);
}