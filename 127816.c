static ippp_bundle *isdn_ppp_mp_bundle_alloc(void)
{
	int i;
	for (i = 0; i < ISDN_MAX_CHANNELS; i++)
		if (isdn_ppp_bundle_arr[i].ref_ct <= 0)
			return (isdn_ppp_bundle_arr + i);
	return NULL;
}