static void gf_m2ts_store_temi(GF_M2TS_Demuxer *ts, GF_M2TS_PES *pes)
{
	GF_BitStream *bs = gf_bs_new(pes->temi_tc_desc, pes->temi_tc_desc_len, GF_BITSTREAM_READ);
	u32 has_timestamp = gf_bs_read_int(bs, 2);
	Bool has_ntp = (Bool) gf_bs_read_int(bs, 1);
	/*u32 has_ptp = */gf_bs_read_int(bs, 1);
	/*u32 has_timecode = */gf_bs_read_int(bs, 2);

	memset(&pes->temi_tc, 0, sizeof(GF_M2TS_TemiTimecodeDescriptor));
	pes->temi_tc.force_reload = gf_bs_read_int(bs, 1);
	pes->temi_tc.is_paused = gf_bs_read_int(bs, 1);
	pes->temi_tc.is_discontinuity = gf_bs_read_int(bs, 1);
	gf_bs_read_int(bs, 7);
	pes->temi_tc.timeline_id = gf_bs_read_int(bs, 8);
	if (has_timestamp) {
		pes->temi_tc.media_timescale = gf_bs_read_u32(bs);
		if (has_timestamp==2)
			pes->temi_tc.media_timestamp = gf_bs_read_u64(bs);
		else
			pes->temi_tc.media_timestamp = gf_bs_read_u32(bs);
	}
	if (has_ntp) {
		pes->temi_tc.ntp = gf_bs_read_u64(bs);
	}
	gf_bs_del(bs);
	pes->temi_tc_desc_len = 0;
	pes->temi_pending = 1;
}