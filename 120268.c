
GF_Box *mhac_New()
{
	ISOM_DECL_BOX_ALLOC(GF_MHAConfigBox, GF_ISOM_BOX_TYPE_MHAC);
	tmp->configuration_version = 1;
	return (GF_Box *)tmp;