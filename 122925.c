GF_Err vvcc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_VVCConfigurationBox *ptr = (GF_VVCConfigurationBox *) s;
	if (!s) return GF_BAD_PARAM;
	if (!ptr->config) return GF_OK;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	return gf_odf_vvc_cfg_write_bs(ptr->config, bs);
}