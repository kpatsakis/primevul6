static GF_Err gf_m4v_parse_config_mpeg4(GF_M4VParser *m4v, GF_M4VDecSpecInfo *dsi)
{
	s32 o_type;
	u8 go;

	if (!m4v || !dsi) return GF_BAD_PARAM;

	memset(dsi, 0, sizeof(GF_M4VDecSpecInfo));

	go = 1;
	while (go) {
		o_type = M4V_LoadObject(m4v);
		switch (o_type) {
			/*vosh*/
		case M4V_VOS_START_CODE:
			dsi->VideoPL = (u8)gf_bs_read_u8(m4v->bs);
			break;

		case M4V_VOL_START_CODE:
			gf_m4v_parse_vol(m4v, dsi);
			/*shape will be done later*/
			gf_bs_align(m4v->bs);
			break;

		case M4V_VOP_START_CODE:
		case M4V_GOV_START_CODE:
			go = 0;
			break;
			/*EOS*/
		case -1:
			m4v->current_object_start = gf_bs_get_position(m4v->bs);
			return GF_EOS;
			/*don't interest us*/
		case M4V_UDTA_START_CODE:
		default:
			break;
		}
	}
	return GF_OK;
}