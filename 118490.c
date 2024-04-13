void gf_m2ts_reset_parsers_for_program(GF_M2TS_Demuxer *ts, GF_M2TS_Program *prog)
{
	u32 i;

	for (i=0; i<GF_M2TS_MAX_STREAMS; i++) {
		GF_M2TS_ES *es = (GF_M2TS_ES *) ts->ess[i];
		if (!es) continue;
		if (prog && (es->program != prog) ) continue;

		if (es->flags & GF_M2TS_ES_IS_SECTION) {
			GF_M2TS_SECTION_ES *ses = (GF_M2TS_SECTION_ES *)es;
			gf_m2ts_section_filter_reset(ses->sec);
		} else {
			GF_M2TS_PES *pes = (GF_M2TS_PES *)es;
			if (!pes || (pes->pid==pes->program->pmt_pid)) continue;
			pes->cc = -1;
			pes->frame_state = 0;
			pes->pck_data_len = 0;
			if (pes->prev_data) gf_free(pes->prev_data);
			pes->prev_data = NULL;
			pes->prev_data_len = 0;
			pes->PTS = pes->DTS = 0;
//			pes->prev_PTS = 0;
//			pes->first_dts = 0;
			pes->pes_len = pes->pes_end_packet_number = pes->pes_start_packet_number = 0;
			if (pes->buf) gf_free(pes->buf);
			pes->buf = NULL;
			if (pes->temi_tc_desc) gf_free(pes->temi_tc_desc);
			pes->temi_tc_desc = NULL;
			pes->temi_tc_desc_len = pes->temi_tc_desc_alloc_size = 0;

			pes->before_last_pcr_value = pes->before_last_pcr_value_pck_number = 0;
			pes->last_pcr_value = pes->last_pcr_value_pck_number = 0;
			if (pes->program->pcr_pid==pes->pid) {
				pes->program->last_pcr_value = pes->program->last_pcr_value_pck_number = 0;
				pes->program->before_last_pcr_value = pes->program->before_last_pcr_value_pck_number = 0;
			}
		}
	}
}