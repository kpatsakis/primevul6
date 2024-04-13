spell_suggest_intern(suginfo_T *su, int interactive)
{
    // Load the .sug file(s) that are available and not done yet.
    suggest_load_files();

    // 1. Try special cases, such as repeating a word: "the the" -> "the".
    //
    // Set a maximum score to limit the combination of operations that is
    // tried.
    suggest_try_special(su);

    // 2. Try inserting/deleting/swapping/changing a letter, use REP entries
    //    from the .aff file and inserting a space (split the word).
    suggest_try_change(su);

    // For the resulting top-scorers compute the sound-a-like score.
    if (sps_flags & SPS_DOUBLE)
	score_comp_sal(su);

    // 3. Try finding sound-a-like words.
    if ((sps_flags & SPS_FAST) == 0)
    {
	if (sps_flags & SPS_BEST)
	    // Adjust the word score for the suggestions found so far for how
	    // they sound like.
	    rescore_suggestions(su);

	// While going through the soundfold tree "su_maxscore" is the score
	// for the soundfold word, limits the changes that are being tried,
	// and "su_sfmaxscore" the rescored score, which is set by
	// cleanup_suggestions().
	// First find words with a small edit distance, because this is much
	// faster and often already finds the top-N suggestions.  If we didn't
	// find many suggestions try again with a higher edit distance.
	// "sl_sounddone" is used to avoid doing the same word twice.
	suggest_try_soundalike_prep();
	su->su_maxscore = SCORE_SFMAX1;
	su->su_sfmaxscore = SCORE_MAXINIT * 3;
	suggest_try_soundalike(su);
	if (su->su_ga.ga_len < SUG_CLEAN_COUNT(su))
	{
	    // We didn't find enough matches, try again, allowing more
	    // changes to the soundfold word.
	    su->su_maxscore = SCORE_SFMAX2;
	    suggest_try_soundalike(su);
	    if (su->su_ga.ga_len < SUG_CLEAN_COUNT(su))
	    {
		// Still didn't find enough matches, try again, allowing even
		// more changes to the soundfold word.
		su->su_maxscore = SCORE_SFMAX3;
		suggest_try_soundalike(su);
	    }
	}
	su->su_maxscore = su->su_sfmaxscore;
	suggest_try_soundalike_finish();
    }

    // When CTRL-C was hit while searching do show the results.  Only clear
    // got_int when using a command, not for spellsuggest().
    ui_breakcheck();
    if (interactive && got_int)
    {
	(void)vgetc();
	got_int = FALSE;
    }

    if ((sps_flags & SPS_DOUBLE) == 0 && su->su_ga.ga_len != 0)
    {
	if (sps_flags & SPS_BEST)
	    // Adjust the word score for how it sounds like.
	    rescore_suggestions(su);

	// Remove bogus suggestions, sort and truncate at "maxcount".
	check_suggestions(su, &su->su_ga);
	(void)cleanup_suggestions(&su->su_ga, su->su_maxscore, su->su_maxcount);
    }
}