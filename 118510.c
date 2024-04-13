static void gf_m2ts_process_pes(GF_M2TS_Demuxer *ts, GF_M2TS_PES *pes, GF_M2TS_Header *hdr, unsigned char *data, u32 data_size, GF_M2TS_AdaptationField *paf)
{
	u8 expect_cc;
	Bool disc=0;
	Bool flush_pes = 0;

	/*duplicated packet, NOT A DISCONTINUITY, we should discard the packet - however we may encounter this configuration in DASH at segment boundaries.
	If payload start is set, ignore duplication*/
	if (hdr->continuity_counter==pes->cc) {
		if (!hdr->payload_start || (hdr->adaptation_field!=3) ) {
			GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[MPEG-2 TS] PES %d: Duplicated Packet found (CC %d) - skipping\n", pes->pid, pes->cc));
			return;
		}
	} else {
		expect_cc = (pes->cc<0) ? hdr->continuity_counter : (pes->cc + 1) & 0xf;
		if (expect_cc != hdr->continuity_counter)
			disc = 1;
	}
	pes->cc = hdr->continuity_counter;

	if (disc) {
		if (pes->flags & GF_M2TS_ES_IGNORE_NEXT_DISCONTINUITY) {
			pes->flags &= ~GF_M2TS_ES_IGNORE_NEXT_DISCONTINUITY;
			disc = 0;
		}
		if (disc) {
			if (hdr->payload_start) {
				if (pes->pck_data_len) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PES %d: Packet discontinuity (%d expected - got %d) - may have lost end of previous PES\n", pes->pid, expect_cc, hdr->continuity_counter));
				}
			} else {
				if (pes->pck_data_len) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] PES %d: Packet discontinuity (%d expected - got %d) - trashing PES packet\n", pes->pid, expect_cc, hdr->continuity_counter));
				}
				pes->pck_data_len = 0;
				pes->pes_len = 0;
				pes->cc = -1;
				return;
			}
		}
	}

	if (!pes->reframe) return;

	if (hdr->payload_start) {
		flush_pes = 1;
		pes->pes_start_packet_number = ts->pck_number;
		pes->before_last_pcr_value = pes->program->before_last_pcr_value;
		pes->before_last_pcr_value_pck_number = pes->program->before_last_pcr_value_pck_number;
		pes->last_pcr_value = pes->program->last_pcr_value;
		pes->last_pcr_value_pck_number = pes->program->last_pcr_value_pck_number;
	} else if (pes->pes_len && (pes->pck_data_len + data_size == pes->pes_len + 6)) {
		/* 6 = startcode+stream_id+length*/
		/*reassemble pes*/
		if (pes->pck_data_len + data_size > pes->pck_alloc_len) {
			pes->pck_alloc_len = pes->pck_data_len + data_size;
			pes->pck_data = (u8*)gf_realloc(pes->pck_data, pes->pck_alloc_len);
		}
		memcpy(pes->pck_data+pes->pck_data_len, data, data_size);
		pes->pck_data_len += data_size;
		/*force discard*/
		data_size = 0;
		flush_pes = 1;
	}

	/*PES first fragment: flush previous packet*/
	if (flush_pes && pes->pck_data_len) {
		gf_m2ts_flush_pes(ts, pes);
		if (!data_size) return;
	}
	/*we need to wait for first packet of PES*/
	if (!pes->pck_data_len && !hdr->payload_start) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d: Waiting for PES header, trashing data\n", hdr->pid));
		return;
	}
	/*reassemble*/
	if (pes->pck_data_len + data_size > pes->pck_alloc_len ) {
		pes->pck_alloc_len = pes->pck_data_len + data_size;
		pes->pck_data = (u8*)gf_realloc(pes->pck_data, pes->pck_alloc_len);
	}
	memcpy(pes->pck_data + pes->pck_data_len, data, data_size);
	pes->pck_data_len += data_size;

	if (paf && paf->random_access_indicator) pes->rap = 1;
	if (hdr->payload_start && !pes->pes_len && (pes->pck_data_len>=6)) {
		pes->pes_len = (pes->pck_data[4]<<8) | pes->pck_data[5];
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d: Got PES packet len %d\n", pes->pid, pes->pes_len));

		if (pes->pes_len + 6 == pes->pck_data_len) {
			gf_m2ts_flush_pes(ts, pes);
		}
	}
}