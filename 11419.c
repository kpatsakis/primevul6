sug_compare(const void *s1, const void *s2)
{
    suggest_T	*p1 = (suggest_T *)s1;
    suggest_T	*p2 = (suggest_T *)s2;
    int		n = p1->st_score - p2->st_score;

    if (n == 0)
    {
	n = p1->st_altscore - p2->st_altscore;
	if (n == 0)
	    n = STRICMP(p1->st_word, p2->st_word);
    }
    return n;
}