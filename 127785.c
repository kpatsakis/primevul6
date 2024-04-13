static int isdn_ppp_mp_bundle_array_init(void)
{
	int i;
	int sz = ISDN_MAX_CHANNELS * sizeof(ippp_bundle);
	if ((isdn_ppp_bundle_arr = kzalloc(sz, GFP_KERNEL)) == NULL)
		return -ENOMEM;
	for (i = 0; i < ISDN_MAX_CHANNELS; i++)
		spin_lock_init(&isdn_ppp_bundle_arr[i].lock);
	return 0;
}