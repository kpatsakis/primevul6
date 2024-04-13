
GF_Err rvcc_Read(GF_Box *s,GF_BitStream *bs)
{
	GF_RVCConfigurationBox *ptr = (GF_RVCConfigurationBox*)s;
	ptr->predefined_rvc_config = gf_bs_read_u16(bs);
	ISOM_DECREASE_SIZE(ptr, 2);
	if (!ptr->predefined_rvc_config) {
		ptr->rvc_meta_idx = gf_bs_read_u16(bs);
		ISOM_DECREASE_SIZE(ptr, 2);
	}
	return GF_OK;