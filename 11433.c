rescore_suggestions(suginfo_T *su)
{
    int		i;

    if (su->su_sallang != NULL)
	for (i = 0; i < su->su_ga.ga_len; ++i)
	    rescore_one(su, &SUG(su->su_ga, i));
}