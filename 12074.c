efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    if (vim_strchr((char_u *)"+-", *efmp) != NULL)
	efminfo->flags = *efmp++;
    if (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != NULL)
	efminfo->prefix = *efmp;
    else
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	return NULL;
    }

    return efmp;
}