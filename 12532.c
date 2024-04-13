cdf_swap_class(cdf_classid_t *d)
{
	d->cl_dword = CDF_TOLE4(d->cl_dword);
	d->cl_word[0] = CDF_TOLE2(d->cl_word[0]);
	d->cl_word[1] = CDF_TOLE2(d->cl_word[1]);
}