str2special_save(
    char_u  *str,
    int	    is_lhs)  // TRUE for lhs, FALSE for rhs
{
    garray_T	ga;
    char_u	*p = str;

    ga_init2(&ga, 1, 40);
    while (*p != NUL)
	ga_concat(&ga, str2special(&p, is_lhs));
    ga_append(&ga, NUL);
    return (char_u *)ga.ga_data;
}