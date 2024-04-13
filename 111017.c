GF_Err gf_m4v_parse_config(GF_M4VParser *m4v, GF_M4VDecSpecInfo *dsi)
{
	if (m4v->mpeg12) {
		return gf_m4v_parse_config_mpeg12(m4v, dsi);
	}
	else {
		return gf_m4v_parse_config_mpeg4(m4v, dsi);
	}
}