void gf_m2ts_reset_parsers(GF_M2TS_Demuxer *ts)
{
	gf_m2ts_reset_parsers_for_program(ts, NULL);

	ts->pck_number = 0;

	gf_m2ts_section_filter_reset(ts->cat);
	gf_m2ts_section_filter_reset(ts->pat);
	gf_m2ts_section_filter_reset(ts->sdt);
	gf_m2ts_section_filter_reset(ts->nit);
	gf_m2ts_section_filter_reset(ts->eit);
	gf_m2ts_section_filter_reset(ts->tdt_tot);

}