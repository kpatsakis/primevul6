static GF_Err gf_m2ts_process_packet(GF_M2TS_Demuxer *ts, unsigned char *data)
{
	GF_M2TS_ES *es;
	GF_M2TS_Header hdr;
	GF_M2TS_AdaptationField af, *paf;
	u32 payload_size, af_size;
	u32 pos = 0;

	ts->pck_number++;

	/* read TS packet header*/
	hdr.sync = data[0];
	if (hdr.sync != 0x47) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] TS Packet %d does not start with sync marker\n", ts->pck_number));
		return GF_CORRUPTED_DATA;
	}
	hdr.error = (data[1] & 0x80) ? 1 : 0;
	hdr.payload_start = (data[1] & 0x40) ? 1 : 0;
	hdr.priority = (data[1] & 0x20) ? 1 : 0;
	hdr.pid = ( (data[1]&0x1f) << 8) | data[2];
	hdr.scrambling_ctrl = (data[3] >> 6) & 0x3;
	hdr.adaptation_field = (data[3] >> 4) & 0x3;
	hdr.continuity_counter = data[3] & 0xf;

	if (hdr.error) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] TS Packet %d has error (PID could be %d)\n", ts->pck_number, hdr.pid));
		return GF_CORRUPTED_DATA;
	}
//#if DEBUG_TS_PACKET
	GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] TS Packet %d PID %d CC %d Encrypted %d\n", ts->pck_number, hdr.pid, hdr.continuity_counter, hdr.scrambling_ctrl));
//#endif

	if (hdr.scrambling_ctrl) {
		//TODO add decyphering
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] TS Packet %d is scrambled - not supported\n", ts->pck_number, hdr.pid));
		return GF_NOT_SUPPORTED;
	}

	paf = NULL;
	payload_size = 184;
	pos = 4;
	switch (hdr.adaptation_field) {
	/*adaptation+data*/
	case 3:
		af_size = data[4];
		if (af_size>183) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] TS Packet %d AF field larger than 183 !\n", ts->pck_number));
			//error
			return GF_CORRUPTED_DATA;
		}
		paf = &af;
		memset(paf, 0, sizeof(GF_M2TS_AdaptationField));
		//this will stop you when processing invalid (yet existing) mpeg2ts streams in debug
		assert( af_size<=183);
		if (af_size>183)
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] TS Packet %d Detected wrong adaption field size %u when control value is 3\n", ts->pck_number, af_size));
		if (af_size) gf_m2ts_get_adaptation_field(ts, paf, data+5, af_size, hdr.pid);
		pos += 1+af_size;
		payload_size = 183 - af_size;
		break;
	/*adaptation only - still process in case of PCR*/
	case 2:
		af_size = data[4];
		if (af_size != 183) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] TS Packet %d AF size is %d when it must be 183 for AF type 2\n", ts->pck_number, af_size));
			return GF_CORRUPTED_DATA;
		}
		paf = &af;
		memset(paf, 0, sizeof(GF_M2TS_AdaptationField));
		gf_m2ts_get_adaptation_field(ts, paf, data+5, af_size, hdr.pid);
		payload_size = 0;
		/*no payload and no PCR, return*/
		if (!paf->PCR_flag)
			return GF_OK;
		break;
	/*reserved*/
	case 0:
		return GF_OK;
	default:
		break;
	}
	data += pos;

	/*PAT*/
	if (hdr.pid == GF_M2TS_PID_PAT) {
		gf_m2ts_gather_section(ts, ts->pat, NULL, &hdr, data, payload_size);
		return GF_OK;
	} else if (hdr.pid == GF_M2TS_PID_CAT) {
		gf_m2ts_gather_section(ts, ts->cat, NULL, &hdr, data, payload_size);
		return GF_OK;
	}

	es = ts->ess[hdr.pid];
	if (paf && paf->PCR_flag) {
		if (!es) {
			u32 i, j;
			for(i=0; i<gf_list_count(ts->programs); i++) {
				GF_M2TS_PES *first_pes = NULL;
				GF_M2TS_Program *program = (GF_M2TS_Program *)gf_list_get(ts->programs,i);
				if(program->pcr_pid != hdr.pid) continue;
				for (j=0; j<gf_list_count(program->streams); j++) {
					GF_M2TS_PES *pes = (GF_M2TS_PES *) gf_list_get(program->streams, j);
					if (pes->flags & GF_M2TS_INHERIT_PCR) {
						ts->ess[hdr.pid] = (GF_M2TS_ES *) pes;
						pes->flags |= GF_M2TS_FAKE_PCR;
						break;
					}
					if (pes->flags & GF_M2TS_ES_IS_PES) {
						first_pes = pes;
					}
				}
				//non found, use the first media stream as a PCR destination - Q: is it legal to have PCR only streams not declared in PMT ?
				if (!es && first_pes) {
					es = (GF_M2TS_ES *) first_pes;
					first_pes->flags |= GF_M2TS_FAKE_PCR;
				}
				break;
			}
			if (!es)
				es = ts->ess[hdr.pid];
		}
		if (es) {
			GF_M2TS_PES_PCK pck;
			s64 prev_diff_in_us;
			Bool discontinuity;
			s32 cc = -1;

			if (es->flags & GF_M2TS_FAKE_PCR) {
				cc = es->program->pcr_cc;
				es->program->pcr_cc = hdr.continuity_counter;
			}
			else if (es->flags & GF_M2TS_ES_IS_PES) cc = ((GF_M2TS_PES*)es)->cc;
			else if (((GF_M2TS_SECTION_ES*)es)->sec) cc = ((GF_M2TS_SECTION_ES*)es)->sec->cc;

			discontinuity = paf->discontinuity_indicator;
			if ((cc>=0) && es->program->before_last_pcr_value) {
				//no increment of CC if AF only packet
				if (hdr.adaptation_field == 2) {
					if (hdr.continuity_counter != cc) {
						discontinuity = GF_TRUE;
					}
				} else if (hdr.continuity_counter != ((cc + 1) & 0xF)) {
					discontinuity = GF_TRUE;
				}
			}

			memset(&pck, 0, sizeof(GF_M2TS_PES_PCK));
			prev_diff_in_us = (s64) (es->program->last_pcr_value /27- es->program->before_last_pcr_value/27);
			es->program->before_last_pcr_value = es->program->last_pcr_value;
			es->program->before_last_pcr_value_pck_number = es->program->last_pcr_value_pck_number;
			es->program->last_pcr_value_pck_number = ts->pck_number;
			es->program->last_pcr_value = paf->PCR_base * 300 + paf->PCR_ext;
			if (!es->program->last_pcr_value) es->program->last_pcr_value =  1;

			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d PCR found "LLU" ("LLU" at 90kHz) - PCR diff is %d us\n", hdr.pid, es->program->last_pcr_value, es->program->last_pcr_value/300, (s32) (es->program->last_pcr_value - es->program->before_last_pcr_value)/27 ));

			pck.PTS = es->program->last_pcr_value;
			pck.stream = (GF_M2TS_PES *)es;

			//try to ignore all discontinuities that are less than 200 ms (seen in some HLS setup ...)
			if (discontinuity) {
				s64 diff_in_us = (s64) (es->program->last_pcr_value - es->program->before_last_pcr_value) / 27;
				u64 diff = ABS(diff_in_us - prev_diff_in_us);

				if ((diff_in_us<0) && (diff_in_us >= -200000)) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d new PCR, with discontinuity signaled, is less than previously received PCR (diff %d us) but not too large, trying to ignore discontinuity\n", hdr.pid, diff_in_us));
				}

				//ignore PCR discontinuity indicator if PCR found is larger than previously received PCR and diffence between PCR before and after discontinuity indicator is smaller than 50ms
				else if ((diff_in_us > 0) && (diff < 200000)) {
					GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d PCR discontinuity signaled but diff is small (diff %d us - PCR diff %d vs prev PCR diff %d) - ignore it\n", hdr.pid, diff, diff_in_us, prev_diff_in_us));
				} else if (paf->discontinuity_indicator) {
					GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d PCR discontinuity signaled (diff %d us - PCR diff %d vs prev PCR diff %d)\n", hdr.pid, diff, diff_in_us, prev_diff_in_us));
					pck.flags = GF_M2TS_PES_PCK_DISCONTINUITY;
				} else {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d PCR discontinuity not signaled (diff %d us - PCR diff %d vs prev PCR diff %d)\n", hdr.pid, diff, diff_in_us, prev_diff_in_us));
					pck.flags = GF_M2TS_PES_PCK_DISCONTINUITY;
				}
			}
			else if ( (es->program->last_pcr_value < es->program->before_last_pcr_value) ) {
				s64 diff_in_us = (s64) (es->program->last_pcr_value - es->program->before_last_pcr_value) / 27;
				//if less than 200 ms before PCR loop at the last PCR, this is a PCR loop
				if (GF_M2TS_MAX_PCR - es->program->before_last_pcr_value < 5400000 /*2*2700000*/) {
					GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d PCR loop found from "LLU" to "LLU" \n", hdr.pid, es->program->before_last_pcr_value, es->program->last_pcr_value));
				} else if ((diff_in_us<0) && (diff_in_us >= -200000)) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d new PCR, without discontinuity signaled, is less than previously received PCR (diff %d us) but not too large, trying to ignore discontinuity\n", hdr.pid, diff_in_us));
				} else {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d PCR found "LLU" is less than previously received PCR "LLU" (PCR diff %g sec) but no discontinuity signaled\n", hdr.pid, es->program->last_pcr_value, es->program->before_last_pcr_value, (GF_M2TS_MAX_PCR - es->program->before_last_pcr_value + es->program->last_pcr_value) / 27000000.0));

					pck.flags = GF_M2TS_PES_PCK_DISCONTINUITY;
				}
			}

			if (pck.flags & GF_M2TS_PES_PCK_DISCONTINUITY) {
				gf_m2ts_reset_parsers_for_program(ts, es->program);
			}

			if (ts->on_event) {
				ts->on_event(ts, GF_M2TS_EVT_PES_PCR, &pck);
			}
		}
	}

	/*check for DVB reserved PIDs*/
	if (!es) {
		if (hdr.pid == GF_M2TS_PID_SDT_BAT_ST) {
			gf_m2ts_gather_section(ts, ts->sdt, NULL, &hdr, data, payload_size);
			return GF_OK;
		} else if (hdr.pid == GF_M2TS_PID_NIT_ST) {
			/*ignore them, unused at application level*/
			gf_m2ts_gather_section(ts, ts->nit, NULL, &hdr, data, payload_size);
			return GF_OK;
		} else if (hdr.pid == GF_M2TS_PID_EIT_ST_CIT) {
			/* ignore EIT messages for the moment */
			gf_m2ts_gather_section(ts, ts->eit, NULL, &hdr, data, payload_size);
			return GF_OK;
		} else if (hdr.pid == GF_M2TS_PID_TDT_TOT_ST) {
			gf_m2ts_gather_section(ts, ts->tdt_tot, NULL, &hdr, data, payload_size);
		} else {
			/* ignore packet */
		}
	} else if (es->flags & GF_M2TS_ES_IS_SECTION) { 	/* The stream uses sections to carry its payload */
		GF_M2TS_SECTION_ES *ses = (GF_M2TS_SECTION_ES *)es;
		if (ses->sec) gf_m2ts_gather_section(ts, ses->sec, ses, &hdr, data, payload_size);
	} else {
		GF_M2TS_PES *pes = (GF_M2TS_PES *)es;
		/* regular stream using PES packets */
		if (pes->reframe && payload_size) gf_m2ts_process_pes(ts, pes, &hdr, data, payload_size, paf);
	}

	return GF_OK;
}