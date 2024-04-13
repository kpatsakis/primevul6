GF_M2TS_Demuxer *gf_m2ts_demux_new()
{
	GF_M2TS_Demuxer *ts;

	GF_SAFEALLOC(ts, GF_M2TS_Demuxer);
	if (!ts) return NULL;
	ts->programs = gf_list_new();
	ts->SDTs = gf_list_new();

	ts->pat = gf_m2ts_section_filter_new(gf_m2ts_process_pat, 0);
	ts->cat = gf_m2ts_section_filter_new(gf_m2ts_process_cat, 0);
	ts->sdt = gf_m2ts_section_filter_new(gf_m2ts_process_sdt, 1);
	ts->nit = gf_m2ts_section_filter_new(gf_m2ts_process_nit, 0);
	ts->eit = gf_m2ts_section_filter_new(NULL/*gf_m2ts_process_eit*/, 1);
	ts->tdt_tot = gf_m2ts_section_filter_new(gf_m2ts_process_tdt_tot, 1);

#ifdef GPAC_ENABLE_MPE
	gf_dvb_mpe_init(ts);
#endif

	ts->nb_prog_pmt_received = 0;
	ts->ChannelAppList = gf_list_new();
	return ts;
}