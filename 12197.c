efm_option_part_len(char_u *efm)
{
    int len;

    for (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	if (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    return len;
}