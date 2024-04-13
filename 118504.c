static u32 gf_m2ts_reframe_reset(GF_M2TS_Demuxer *ts, GF_M2TS_PES *pes, Bool same_pts, unsigned char *data, u32 data_len, GF_M2TS_PESHeader *pes_hdr)
{
	if (pes->pck_data) {
		gf_free(pes->pck_data);
		pes->pck_data = NULL;
	}
	pes->pck_data_len = pes->pck_alloc_len = 0;
	if (pes->prev_data) {
		gf_free(pes->prev_data);
		pes->prev_data = NULL;
	}
	pes->prev_data_len = 0;
	pes->pes_len = 0;
	pes->prev_PTS = 0;
	pes->reframe = NULL;
	pes->cc = -1;
	pes->temi_tc_desc_len = 0;
	return 0;
}