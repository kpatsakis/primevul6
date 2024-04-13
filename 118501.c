static void gf_m2ts_get_adaptation_field(GF_M2TS_Demuxer *ts, GF_M2TS_AdaptationField *paf, unsigned char *data, u32 size, u32 pid)
{
	unsigned char *af_extension;
	paf->discontinuity_indicator = (data[0] & 0x80) ? 1 : 0;
	paf->random_access_indicator = (data[0] & 0x40) ? 1 : 0;
	paf->priority_indicator = (data[0] & 0x20) ? 1 : 0;
	paf->PCR_flag = (data[0] & 0x10) ? 1 : 0;
	paf->OPCR_flag = (data[0] & 0x8) ? 1 : 0;
	paf->splicing_point_flag = (data[0] & 0x4) ? 1 : 0;
	paf->transport_private_data_flag = (data[0] & 0x2) ? 1 : 0;
	paf->adaptation_field_extension_flag = (data[0] & 0x1) ? 1 : 0;

	af_extension = data + 1;
	if (paf->PCR_flag == 1) {
		u32 base = ((u32)data[1] << 24) | ((u32)data[2] << 16) | ((u32)data[3] << 8) | (u32)data[4];
		u64 PCR = (u64) base;
		paf->PCR_base = (PCR << 1) | (data[5] >> 7);
		paf->PCR_ext = ((data[5] & 1) << 8) | data[6];
		af_extension += 6;
	}

	if (paf->adaptation_field_extension_flag) {
		u32 afext_bytes;
		Bool ltw_flag, pwr_flag, seamless_flag, af_desc_not_present;
		if (paf->OPCR_flag) {
			af_extension += 6;
		}
		if (paf->splicing_point_flag) {
			af_extension += 1;
		}
		if (paf->transport_private_data_flag) {
			u32 priv_bytes = af_extension[0];
			af_extension += 1 + priv_bytes;
		}

		afext_bytes = af_extension[0];
		ltw_flag = af_extension[1] & 0x80 ? 1 : 0;
		pwr_flag = af_extension[1] & 0x40 ? 1 : 0;
		seamless_flag = af_extension[1] & 0x20 ? 1 : 0;
		af_desc_not_present = af_extension[1] & 0x10 ? 1 : 0;
		af_extension += 2;
		if (!afext_bytes) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d: Bad Adaptation Extension found\n", pid));
			return;
		}
		afext_bytes-=1;
		if (ltw_flag) {
			af_extension += 2;
			if (afext_bytes<2) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d: Bad Adaptation Extension found\n", pid));
				return;
			}
			afext_bytes-=2;
		}
		if (pwr_flag) {
			af_extension += 3;
			if (afext_bytes<3) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d: Bad Adaptation Extension found\n", pid));
				return;
			}
			afext_bytes-=3;
		}
		if (seamless_flag) {
			af_extension += 3;
			if (afext_bytes<3) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d: Bad Adaptation Extension found\n", pid));
				return;
			}
			afext_bytes-=3;
		}

		if (! af_desc_not_present) {
			while (afext_bytes) {
				GF_BitStream *bs;
				char *desc;
				u8 desc_tag = af_extension[0];
				u8 desc_len = af_extension[1];
				if (!desc_len || (u32) desc_len+2 > afext_bytes) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d: Bad Adaptation Descriptor found (tag %d) size is %d but only %d bytes available\n", pid, desc_tag, desc_len, afext_bytes));
					break;
				}
				desc = (char *) af_extension+2;

				bs = gf_bs_new(desc, desc_len, GF_BITSTREAM_READ);
				switch (desc_tag) {
				case GF_M2TS_AFDESC_LOCATION_DESCRIPTOR:
				{
					Bool use_base_temi_url;
					char URL[255];
					GF_M2TS_TemiLocationDescriptor temi_loc;
					memset(&temi_loc, 0, sizeof(GF_M2TS_TemiLocationDescriptor) );
					temi_loc.reload_external = gf_bs_read_int(bs, 1);
					temi_loc.is_announce = gf_bs_read_int(bs, 1);
					temi_loc.is_splicing = gf_bs_read_int(bs, 1);
					use_base_temi_url = gf_bs_read_int(bs, 1);
					gf_bs_read_int(bs, 5); //reserved
					temi_loc.timeline_id = gf_bs_read_int(bs, 7);
					if (!use_base_temi_url) {
						char *_url = URL;
						u8 scheme = gf_bs_read_int(bs, 8);
						u8 url_len = gf_bs_read_int(bs, 8);
						switch (scheme) {
						case 1:
							strcpy(URL, "http://");
							_url = URL+7;
							break;
						case 2:
							strcpy(URL, "https://");
							_url = URL+8;
							break;
						}
						gf_bs_read_data(bs, _url, url_len);
						_url[url_len] = 0;
					}
					temi_loc.external_URL = URL;

					GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d AF Location descriptor found - URL %s\n", pid, URL));
					if (ts->on_event) ts->on_event(ts, GF_M2TS_EVT_TEMI_LOCATION, &temi_loc);
				}
				break;
				case GF_M2TS_AFDESC_TIMELINE_DESCRIPTOR:
					if (ts->ess[pid] && (ts->ess[pid]->flags & GF_M2TS_ES_IS_PES)) {
						GF_M2TS_PES *pes = (GF_M2TS_PES *) ts->ess[pid];

						if (pes->temi_tc_desc_len)
							gf_m2ts_store_temi(ts, pes);

						if (pes->temi_tc_desc_alloc_size < desc_len) {
							pes->temi_tc_desc = gf_realloc(pes->temi_tc_desc, desc_len);
							pes->temi_tc_desc_alloc_size = desc_len;
						}
						memcpy(pes->temi_tc_desc, desc, desc_len);
						pes->temi_tc_desc_len = desc_len;

						GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d AF Timeline descriptor found\n", pid));
					}
					break;
				}
				gf_bs_del(bs);

				af_extension += 2+desc_len;
				afext_bytes -= 2+desc_len;
			}

		}
	}

	GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d: Adaptation Field found: Discontinuity %d - RAP %d - PCR: "LLD"\n", pid, paf->discontinuity_indicator, paf->random_access_indicator, paf->PCR_flag ? paf->PCR_base * 300 + paf->PCR_ext : 0));
}