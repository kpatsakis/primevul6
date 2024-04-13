ignore_error(char_u *msg)
{
    int i;

    for (i = 0; i < ignore_error_list.ga_len; ++i)
	if (strstr((char *)msg,
		  (char *)((char_u **)(ignore_error_list.ga_data))[i]) != NULL)
	    return TRUE;
    return FALSE;
}