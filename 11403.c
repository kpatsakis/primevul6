spell_find_cleanup(suginfo_T *su)
{
    int		i;

    // Free the suggestions.
    for (i = 0; i < su->su_ga.ga_len; ++i)
	vim_free(SUG(su->su_ga, i).st_word);
    ga_clear(&su->su_ga);
    for (i = 0; i < su->su_sga.ga_len; ++i)
	vim_free(SUG(su->su_sga, i).st_word);
    ga_clear(&su->su_sga);

    // Free the banned words.
    hash_clear_all(&su->su_banned, 0);
}