spell_edit_score_limit(
    slang_T	*slang,
    char_u	*badword,
    char_u	*goodword,
    int		limit)
{
    limitscore_T    stack[10];		// allow for over 3 * 2 edits
    int		    stackidx;
    int		    bi, gi;
    int		    bi2, gi2;
    int		    bc, gc;
    int		    score;
    int		    score_off;
    int		    minscore;
    int		    round;

    // Multi-byte characters require a bit more work, use a different function
    // to avoid testing "has_mbyte" quite often.
    if (has_mbyte)
	return spell_edit_score_limit_w(slang, badword, goodword, limit);

    // The idea is to go from start to end over the words.  So long as
    // characters are equal just continue, this always gives the lowest score.
    // When there is a difference try several alternatives.  Each alternative
    // increases "score" for the edit distance.  Some of the alternatives are
    // pushed unto a stack and tried later, some are tried right away.  At the
    // end of the word the score for one alternative is known.  The lowest
    // possible score is stored in "minscore".
    stackidx = 0;
    bi = 0;
    gi = 0;
    score = 0;
    minscore = limit + 1;

    for (;;)
    {
	// Skip over an equal part, score remains the same.
	for (;;)
	{
	    bc = badword[bi];
	    gc = goodword[gi];
	    if (bc != gc)	// stop at a char that's different
		break;
	    if (bc == NUL)	// both words end
	    {
		if (score < minscore)
		    minscore = score;
		goto pop;	// do next alternative
	    }
	    ++bi;
	    ++gi;
	}

	if (gc == NUL)    // goodword ends, delete badword chars
	{
	    do
	    {
		if ((score += SCORE_DEL) >= minscore)
		    goto pop;	    // do next alternative
	    } while (badword[++bi] != NUL);
	    minscore = score;
	}
	else if (bc == NUL) // badword ends, insert badword chars
	{
	    do
	    {
		if ((score += SCORE_INS) >= minscore)
		    goto pop;	    // do next alternative
	    } while (goodword[++gi] != NUL);
	    minscore = score;
	}
	else			// both words continue
	{
	    // If not close to the limit, perform a change.  Only try changes
	    // that may lead to a lower score than "minscore".
	    // round 0: try deleting a char from badword
	    // round 1: try inserting a char in badword
	    for (round = 0; round <= 1; ++round)
	    {
		score_off = score + (round == 0 ? SCORE_DEL : SCORE_INS);
		if (score_off < minscore)
		{
		    if (score_off + SCORE_EDIT_MIN >= minscore)
		    {
			// Near the limit, rest of the words must match.  We
			// can check that right now, no need to push an item
			// onto the stack.
			bi2 = bi + 1 - round;
			gi2 = gi + round;
			while (goodword[gi2] == badword[bi2])
			{
			    if (goodword[gi2] == NUL)
			    {
				minscore = score_off;
				break;
			    }
			    ++bi2;
			    ++gi2;
			}
		    }
		    else
		    {
			// try deleting/inserting a character later
			stack[stackidx].badi = bi + 1 - round;
			stack[stackidx].goodi = gi + round;
			stack[stackidx].score = score_off;
			++stackidx;
		    }
		}
	    }

	    if (score + SCORE_SWAP < minscore)
	    {
		// If swapping two characters makes a match then the
		// substitution is more expensive, thus there is no need to
		// try both.
		if (gc == badword[bi + 1] && bc == goodword[gi + 1])
		{
		    // Swap two characters, that is: skip them.
		    gi += 2;
		    bi += 2;
		    score += SCORE_SWAP;
		    continue;
		}
	    }

	    // Substitute one character for another which is the same
	    // thing as deleting a character from both goodword and badword.
	    // Use a better score when there is only a case difference.
	    if (SPELL_TOFOLD(bc) == SPELL_TOFOLD(gc))
		score += SCORE_ICASE;
	    else
	    {
		// For a similar character use SCORE_SIMILAR.
		if (slang != NULL
			&& slang->sl_has_map
			&& similar_chars(slang, gc, bc))
		    score += SCORE_SIMILAR;
		else
		    score += SCORE_SUBST;
	    }

	    if (score < minscore)
	    {
		// Do the substitution.
		++gi;
		++bi;
		continue;
	    }
	}
pop:
	// Get here to try the next alternative, pop it from the stack.
	if (stackidx == 0)		// stack is empty, finished
	    break;

	// pop an item from the stack
	--stackidx;
	gi = stack[stackidx].goodi;
	bi = stack[stackidx].badi;
	score = stack[stackidx].score;
    }

    // When the score goes over "limit" it may actually be much higher.
    // Return a very large number to avoid going below the limit when giving a
    // bonus.
    if (minscore > limit)
	return SCORE_MAXMAX;
    return minscore;
}