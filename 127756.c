isdn_ppp_cleanup(void)
{
	int i;

	for (i = 0; i < ISDN_MAX_CHANNELS; i++)
		kfree(ippp_table[i]);

#ifdef CONFIG_ISDN_MPP
	kfree(isdn_ppp_bundle_arr);
#endif /* CONFIG_ISDN_MPP */

}