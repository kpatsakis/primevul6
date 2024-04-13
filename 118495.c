static Bool gf_m2ts_probe_buffer(char *buf, u32 size)
{
	GF_Err e;
	GF_M2TS_Demuxer *ts;
	u32 lt;

	lt = gf_log_get_tool_level(GF_LOG_CONTAINER);
	gf_log_set_tool_level(GF_LOG_CONTAINER, GF_LOG_QUIET);

	ts = gf_m2ts_demux_new();
	e = gf_m2ts_process_data(ts, buf, size);
	if (!ts->pck_number) e = GF_BAD_PARAM;
	gf_m2ts_demux_del(ts);

	gf_log_set_tool_level(GF_LOG_CONTAINER, lt);

	if (e) return GF_FALSE;
	return GF_TRUE;

}