static void gf_m4v_parse_vol(GF_M4VParser *m4v, GF_M4VDecSpecInfo *dsi)
{
	u8 verid, par;
	s32 clock_rate;
	u8 vpl = dsi->VideoPL;

	memset(dsi, 0, sizeof(GF_M4VDecSpecInfo));
	dsi->VideoPL = vpl;

	verid = 0;
	dsi->RAP_stream = gf_bs_read_int(m4v->bs, 1);
	dsi->objectType = gf_bs_read_int(m4v->bs, 8);
	if (gf_bs_read_int(m4v->bs, 1)) {
		verid = gf_bs_read_int(m4v->bs, 4);
		gf_bs_read_int(m4v->bs, 3);
	}
	par = gf_bs_read_int(m4v->bs, 4);
	if (par == 0xF) {
		dsi->par_num = gf_bs_read_int(m4v->bs, 8);
		dsi->par_den = gf_bs_read_int(m4v->bs, 8);
	} else if (par<6) {
		dsi->par_num = m4v_sar[par].w;
		dsi->par_den = m4v_sar[par].h;
	}
	if (gf_bs_read_int(m4v->bs, 1)) {
		gf_bs_read_int(m4v->bs, 3);
		if (gf_bs_read_int(m4v->bs, 1)) gf_bs_read_int(m4v->bs, 79);
	}
	dsi->has_shape = gf_bs_read_int(m4v->bs, 2);
	if (dsi->has_shape && (verid!=1) ) gf_bs_read_int(m4v->bs, 4);
	gf_bs_read_int(m4v->bs, 1);
	/*clock rate*/
	dsi->clock_rate = gf_bs_read_int(m4v->bs, 16);
	/*marker*/
	gf_bs_read_int(m4v->bs, 1);

	clock_rate = dsi->clock_rate-1;
	if (clock_rate >= 65536) clock_rate = 65535;
	if (clock_rate > 0) {
		for (dsi->NumBitsTimeIncrement = 1; dsi->NumBitsTimeIncrement < 16; dsi->NumBitsTimeIncrement++)	{
			if (clock_rate == 1) break;
			clock_rate = (clock_rate >> 1);
		}
	} else {
		/*fix from vivien for divX*/
		dsi->NumBitsTimeIncrement = 1;
	}
	/*fixed FPS stream*/
	dsi->time_increment = 0;
	if (gf_bs_read_int(m4v->bs, 1)) {
		dsi->time_increment = gf_bs_read_int(m4v->bs, dsi->NumBitsTimeIncrement);
	}
	if (!dsi->has_shape) {
		gf_bs_read_int(m4v->bs, 1);
		dsi->width = gf_bs_read_int(m4v->bs, 13);
		gf_bs_read_int(m4v->bs, 1);
		dsi->height = gf_bs_read_int(m4v->bs, 13);
	} else {
		dsi->width = dsi->height = 0;
	}
	gf_bs_align(m4v->bs);
}