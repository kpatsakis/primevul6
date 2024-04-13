GF_Err gf_m2ts_set_pes_framing(GF_M2TS_PES *pes, u32 mode)
{
	if (!pes) return GF_BAD_PARAM;

	GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] Setting pes framing mode of PID %d to %d\n", pes->pid, mode) );
	/*ignore request for section PIDs*/
	if (pes->flags & GF_M2TS_ES_IS_SECTION) {
		if (pes->flags & GF_M2TS_ES_IS_SL) {
			if (mode==GF_M2TS_PES_FRAMING_DEFAULT) {
				((GF_M2TS_SECTION_ES *)pes)->sec->process_section = gf_m2ts_process_mpeg4section;
			} else {
				((GF_M2TS_SECTION_ES *)pes)->sec->process_section = NULL;
			}
		}
		return GF_OK;
	}

	if (pes->pid==pes->program->pmt_pid) return GF_BAD_PARAM;

	//if component reuse, disable previous pes
	if ((mode > GF_M2TS_PES_FRAMING_SKIP) && (pes->program->ts->ess[pes->pid] != (GF_M2TS_ES *) pes)) {
		GF_M2TS_PES *o_pes = (GF_M2TS_PES *) pes->program->ts->ess[pes->pid];
		if (o_pes->flags & GF_M2TS_ES_IS_PES)
			gf_m2ts_set_pes_framing(o_pes, GF_M2TS_PES_FRAMING_SKIP);

		GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[MPEG-2 TS] Reassinging PID %d from program %d to program %d\n", pes->pid, o_pes->program->number, pes->program->number) );
		pes->program->ts->ess[pes->pid] = (GF_M2TS_ES *) pes;
	}

	switch (mode) {
	case GF_M2TS_PES_FRAMING_RAW:
		pes->reframe = gf_m2ts_reframe_default;
		break;
	case GF_M2TS_PES_FRAMING_SKIP:
		pes->reframe = gf_m2ts_reframe_reset;
		break;
	case GF_M2TS_PES_FRAMING_SKIP_NO_RESET:
		pes->reframe = NULL;
		break;
	case GF_M2TS_PES_FRAMING_DEFAULT:
	default:
		switch (pes->stream_type) {
		case GF_M2TS_VIDEO_MPEG1:
		case GF_M2TS_VIDEO_MPEG2:
		case GF_M2TS_VIDEO_H264:
		case GF_M2TS_VIDEO_SVC:
		case GF_M2TS_VIDEO_HEVC:
		case GF_M2TS_VIDEO_HEVC_TEMPORAL:
		case GF_M2TS_VIDEO_HEVC_MCTS:
		case GF_M2TS_VIDEO_SHVC:
		case GF_M2TS_VIDEO_SHVC_TEMPORAL:
		case GF_M2TS_VIDEO_MHVC:
		case GF_M2TS_VIDEO_MHVC_TEMPORAL:
		case GF_M2TS_AUDIO_MPEG1:
		case GF_M2TS_AUDIO_MPEG2:
		case GF_M2TS_AUDIO_AAC:
		case GF_M2TS_AUDIO_LATM_AAC:
		case GF_M2TS_AUDIO_AC3:
		case GF_M2TS_AUDIO_EC3:
			//for all our supported codec types, use a reframer filter
			pes->reframe = gf_m2ts_reframe_default;
			break;

		case GF_M2TS_PRIVATE_DATA:
			/* TODO: handle DVB subtitle streams */
			break;
		case GF_M2TS_METADATA_ID3_HLS:
			//TODO
			pes->reframe = gf_m2ts_reframe_id3_pes;
			break;
		default:
			pes->reframe = gf_m2ts_reframe_default;
			break;
		}
		break;
	}
	return GF_OK;
}