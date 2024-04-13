GF_Err gf_m4v_get_config(u8 *rawdsi, u32 rawdsi_size, GF_M4VDecSpecInfo *dsi)
{
	GF_Err e;
	GF_M4VParser *vparse;
	if (!rawdsi || !rawdsi_size) return GF_NON_COMPLIANT_BITSTREAM;
	vparse = gf_m4v_parser_new(rawdsi, rawdsi_size, 0);
	e = gf_m4v_parse_config(vparse, dsi);
	dsi->next_object_start = (u32)vparse->current_object_start;
	gf_m4v_parser_del(vparse);
	return e < 0 ? e : GF_OK;
}