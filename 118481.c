void gf_m2ts_flush_pes(GF_M2TS_Demuxer *ts, GF_M2TS_PES *pes)
{
	GF_M2TS_PESHeader pesh;
	if (!ts) return;

	/*we need at least a full, valid start code and PES header !!*/
	if ((pes->pck_data_len >= 4) && !pes->pck_data[0] && !pes->pck_data[1] && (pes->pck_data[2] == 0x1)) {
		u32 len;
		Bool has_pes_header = GF_TRUE;
		u32 stream_id = pes->pck_data[3];
		Bool same_pts = GF_FALSE;

		switch (stream_id) {
		case GF_M2_STREAMID_PROGRAM_STREAM_MAP:
		case GF_M2_STREAMID_PADDING:
		case GF_M2_STREAMID_PRIVATE_2:
		case GF_M2_STREAMID_ECM:
		case GF_M2_STREAMID_EMM:
		case GF_M2_STREAMID_PROGRAM_STREAM_DIRECTORY:
		case GF_M2_STREAMID_DSMCC:
		case GF_M2_STREAMID_H222_TYPE_E:
			has_pes_header = GF_FALSE;
			break;
		}

		if (has_pes_header) {

			/*OK read header*/
			gf_m2ts_pes_header(pes, pes->pck_data + 3, pes->pck_data_len - 3, &pesh);

			/*send PES timing*/
			if (ts->notify_pes_timing) {
				GF_M2TS_PES_PCK pck;
				memset(&pck, 0, sizeof(GF_M2TS_PES_PCK));
				pck.PTS = pesh.PTS;
				pck.DTS = pesh.DTS;
				pck.stream = pes;
				if (pes->rap) pck.flags |= GF_M2TS_PES_PCK_RAP;
				pes->pes_end_packet_number = ts->pck_number;
				if (ts->on_event) ts->on_event(ts, GF_M2TS_EVT_PES_TIMING, &pck);
			}
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d Got PES header DTS %d PTS %d\n", pes->pid, pesh.DTS, pesh.PTS));

			if (pesh.PTS) {
				if (pesh.PTS == pes->PTS) {
					same_pts = GF_TRUE;
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d - same PTS "LLU" for two consecutive PES packets \n", pes->pid, pes->PTS));
				}
	#ifndef GPAC_DISABLE_LOG
				/*FIXME - this test should only be done for non bi-directionnally coded media
				else if (pesh.PTS < pes->PTS) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d - PTS "LLU" less than previous packet PTS "LLU"\n", pes->pid, pesh.PTS, pes->PTS) );
				}
				*/
	#endif

				pes->PTS = pesh.PTS;
	#ifndef GPAC_DISABLE_LOG
				{
					if (pes->DTS && (pesh.DTS == pes->DTS)) {
						GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d - same DTS "LLU" for two consecutive PES packets \n", pes->pid, pes->DTS));
					}
					if (pesh.DTS < pes->DTS) {
						GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d - DTS "LLU" less than previous DTS "LLU"\n", pes->pid, pesh.DTS, pes->DTS));
					}
				}
	#endif
				pes->DTS = pesh.DTS;
			}
			/*no PTSs were coded, same time*/
			else if (!pesh.hdr_data_len) {
				same_pts = GF_TRUE;
			}


			/*3-byte start-code + 6 bytes header + hdr extensions*/
			len = 9 + pesh.hdr_data_len;

		} else {
			/*3-byte start-code + 1 byte streamid*/
			len = 4;
			memset(&pesh, 0, sizeof(pesh));
		}

		if ((u8) pes->pck_data[3]==0xfa) {
			GF_M2TS_SL_PCK sl_pck;

			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] SL Packet in PES for %d - ES ID %d\n", pes->pid, pes->mpeg4_es_id));

			if (pes->pck_data_len > len) {
				sl_pck.data = (char *)pes->pck_data + len;
				sl_pck.data_len = pes->pck_data_len - len;
				sl_pck.stream = (GF_M2TS_ES *)pes;
				if (ts->on_event) ts->on_event(ts, GF_M2TS_EVT_SL_PCK, &sl_pck);
			} else {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] Bad SL Packet size: (%d indicated < %d header)\n", pes->pid, pes->pck_data_len, len));
			}
		} else if (pes->reframe) {
			u32 remain = 0;
			u32 offset = len;

			if (pesh.pck_len && (pesh.pck_len-3-pesh.hdr_data_len != pes->pck_data_len-len)) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d PES payload size %d but received %d bytes\n", pes->pid, (u32) ( pesh.pck_len-3-pesh.hdr_data_len), pes->pck_data_len-len));
			}
			//copy over the remaining of previous PES payload before start of this PES payload
			if (pes->prev_data_len) {
				if (pes->prev_data_len < len) {
					offset = len - pes->prev_data_len;
					memcpy(pes->pck_data + offset, pes->prev_data, pes->prev_data_len);
				} else {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d PES reassembly buffer overflow (%d bytes not processed from previous PES) - discarding prev data\n", pes->pid, pes->prev_data_len ));
				}
			}

			if (!pes->temi_pending && pes->temi_tc_desc_len) {
				gf_m2ts_store_temi(ts, pes);
			}

			if (pes->temi_pending) {
				pes->temi_pending = 0;
				pes->temi_tc.pes_pts = pes->PTS;
				if (ts->on_event)
					ts->on_event(ts, GF_M2TS_EVT_TEMI_TIMECODE, &pes->temi_tc);
			}

			if (! ts->seek_mode)
				remain = pes->reframe(ts, pes, same_pts, pes->pck_data+offset, pes->pck_data_len-offset, &pesh);

			//CLEANUP alloc stuff
			if (pes->prev_data) gf_free(pes->prev_data);
			pes->prev_data = NULL;
			pes->prev_data_len = 0;
			if (remain) {
				pes->prev_data = gf_malloc(sizeof(char)*remain);
				assert(pes->pck_data_len >= remain);
				memcpy(pes->prev_data, pes->pck_data + pes->pck_data_len - remain, remain);
				pes->prev_data_len = remain;
			}
		}
	} else if (pes->pck_data_len) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PES %d: Bad PES Header, discarding packet (maybe stream is encrypted ?)\n", pes->pid));
	}
	pes->pck_data_len = 0;
	pes->pes_len = 0;
	pes->rap = 0;
}