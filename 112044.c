const GF_FilterRegister *xviddec_register(GF_FilterSession *session)
{
#if !defined(GPAC_DISABLE_AV_PARSERS) && defined(GPAC_HAS_XVID)
	return &XVIDRegister;
#else
	return NULL;
#endif
}