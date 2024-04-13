static void gf_m2ts_process_sdt(GF_M2TS_Demuxer *ts, GF_M2TS_SECTION_ES *ses, GF_List *sections, u8 table_id, u16 ex_table_id, u8 version_number, u8 last_section_number, u32 status)
{
	u32 pos, evt_type;
	u32 nb_sections;
	u32 data_size;
	unsigned char *data;
	GF_M2TS_Section *section;

	/*wait for the last section */
	if (!(status&GF_M2TS_TABLE_END)) return;

	/*skip if already received*/
	if (status&GF_M2TS_TABLE_REPEAT) {
		if (ts->on_event) ts->on_event(ts, GF_M2TS_EVT_SDT_REPEAT, NULL);
		return;
	}

	if (table_id != GF_M2TS_TABLE_ID_SDT_ACTUAL) {
		return;
	}

	gf_m2ts_reset_sdt(ts);

	nb_sections = gf_list_count(sections);
	if (nb_sections > 1) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] SDT on multiple sections not supported\n"));
	}

	section = (GF_M2TS_Section *)gf_list_get(sections, 0);
	data = section->data;
	data_size = section->data_size;

	//orig_net_id = (data[0] << 8) | data[1];
	pos = 3;
	while (pos < data_size) {
		GF_M2TS_SDT *sdt;
		u32 descs_size, d_pos, ulen;

		GF_SAFEALLOC(sdt, GF_M2TS_SDT);
		if (!sdt) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] Fail to create SDT\n"));
			return;
		}
		gf_list_add(ts->SDTs, sdt);

		sdt->service_id = (data[pos]<<8) + data[pos+1];
		sdt->EIT_schedule = (data[pos+2] & 0x2) ? 1 : 0;
		sdt->EIT_present_following = (data[pos+2] & 0x1);
		sdt->running_status = (data[pos+3]>>5) & 0x7;
		sdt->free_CA_mode = (data[pos+3]>>4) & 0x1;
		descs_size = ((data[pos+3]&0xf)<<8) | data[pos+4];
		pos += 5;

		d_pos = 0;
		while (d_pos < descs_size) {
			u8 d_tag = data[pos+d_pos];
			u8 d_len = data[pos+d_pos+1];

			switch (d_tag) {
			case GF_M2TS_DVB_SERVICE_DESCRIPTOR:
				if (sdt->provider) gf_free(sdt->provider);
				sdt->provider = NULL;
				if (sdt->service) gf_free(sdt->service);
				sdt->service = NULL;

				d_pos+=2;
				sdt->service_type = data[pos+d_pos];
				ulen = data[pos+d_pos+1];
				d_pos += 2;
				sdt->provider = (char*)gf_malloc(sizeof(char)*(ulen+1));
				memcpy(sdt->provider, data+pos+d_pos, sizeof(char)*ulen);
				sdt->provider[ulen] = 0;
				d_pos += ulen;

				ulen = data[pos+d_pos];
				d_pos += 1;
				sdt->service = (char*)gf_malloc(sizeof(char)*(ulen+1));
				memcpy(sdt->service, data+pos+d_pos, sizeof(char)*ulen);
				sdt->service[ulen] = 0;
				d_pos += ulen;
				break;

			default:
				GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] Skipping descriptor (0x%x) not supported\n", d_tag));
				d_pos += d_len;
				if (d_len == 0) d_pos = descs_size;
				break;
			}
		}
		pos += descs_size;
	}
	evt_type = GF_M2TS_EVT_SDT_FOUND;
	if (ts->on_event) ts->on_event(ts, evt_type, NULL);
}