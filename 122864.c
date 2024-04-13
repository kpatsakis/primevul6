GF_Err hvcc_box_read(GF_Box *s, GF_BitStream *bs)
{
	u64 consumed;
	GF_HEVCConfigurationBox *ptr = (GF_HEVCConfigurationBox *)s;

	if (ptr->config) gf_odf_hevc_cfg_del(ptr->config);

	consumed = gf_bs_get_position(bs);
	switch (s->type) {
	case GF_ISOM_BOX_TYPE_HVCC:
	case GF_ISOM_BOX_TYPE_HVCE:
		ptr->config = gf_odf_hevc_cfg_read_bs(bs, GF_FALSE);
		break;
	default:
		ptr->config = gf_odf_hevc_cfg_read_bs(bs, GF_TRUE);
		break;
	}
	consumed = gf_bs_get_position(bs) - consumed ;
	ISOM_DECREASE_SIZE(ptr, (u32)consumed)

	return ptr->config ? GF_OK : GF_ISOM_INVALID_FILE;
}