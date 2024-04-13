static void gf_m2ts_gather_section(GF_M2TS_Demuxer *ts, GF_M2TS_SectionFilter *sec, GF_M2TS_SECTION_ES *ses, GF_M2TS_Header *hdr, unsigned char *data, u32 data_size)
{
	u32 payload_size = data_size;
	u8 expect_cc = (sec->cc<0) ? hdr->continuity_counter : (sec->cc + 1) & 0xf;
	Bool disc = (expect_cc == hdr->continuity_counter) ? 0 : 1;
	sec->cc = expect_cc;

	/*may happen if hdr->adaptation_field=2 no payload in TS packet*/
	if (!data_size) return;

	if (hdr->payload_start) {
		u32 ptr_field;

		ptr_field = data[0];
		if (ptr_field+1>data_size) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] Invalid section start (@ptr_field=%d, @data_size=%d)\n", ptr_field, data_size) );
			return;
		}

		/*end of previous section*/
		if (!sec->length && sec->received) {
			/* the length of the section could not be determined from the previous TS packet because we had only 1 or 2 bytes */
			if (sec->received == 1)
				sec->length = gf_m2ts_get_section_length(sec->section[0], data[1], data[2]);
			else /* (sec->received == 2)  */
				sec->length = gf_m2ts_get_section_length(sec->section[0], sec->section[1], data[1]);
			sec->section = (char*)gf_realloc(sec->section, sizeof(char)*sec->length);
		}

		if (sec->length && sec->received + ptr_field >= sec->length) {
			u32 len = sec->length - sec->received;
			memcpy(sec->section + sec->received, data+1, sizeof(char)*len);
			sec->received += len;
			if (ptr_field > len)
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] Invalid pointer field (@ptr_field=%d, @remaining=%d)\n", ptr_field, len) );
			gf_m2ts_section_complete(ts, sec, ses);
		}
		data += ptr_field+1;
		data_size -= ptr_field+1;
		payload_size -= ptr_field+1;

aggregated_section:

		if (sec->section) gf_free(sec->section);
		sec->length = sec->received = 0;
		sec->section = (char*)gf_malloc(sizeof(char)*data_size);
		memcpy(sec->section, data, sizeof(char)*data_size);
		sec->received = data_size;
	} else if (disc) {
		if (sec->section) gf_free(sec->section);
		sec->section = NULL;
		sec->received = sec->length = 0;
		return;
	} else if (!sec->section) {
		return;
	} else {
		if (sec->length && sec->received+data_size > sec->length)
			data_size = sec->length - sec->received;

		if (sec->length) {
			memcpy(sec->section + sec->received, data, sizeof(char)*data_size);
		} else {
			sec->section = (char*)gf_realloc(sec->section, sizeof(char)*(sec->received+data_size));
			memcpy(sec->section + sec->received, data, sizeof(char)*data_size);
		}
		sec->received += data_size;
	}
	/*alloc final buffer*/
	if (!sec->length && (sec->received >= 3)) {
		sec->length = gf_m2ts_get_section_length(sec->section[0], sec->section[1], sec->section[2]);
		sec->section = (char*)gf_realloc(sec->section, sizeof(char)*sec->length);

		if (sec->received > sec->length) {
			data_size -= sec->received - sec->length;
			sec->received = sec->length;
		}
	}
	if (!sec->length || sec->received < sec->length) return;

	/*OK done*/
	gf_m2ts_section_complete(ts, sec, ses);

	if (payload_size > data_size) {
		data += data_size;
		/* detect padding after previous section */
		if (data[0] != 0xFF) {
			data_size = payload_size - data_size;
			payload_size = data_size;
			goto aggregated_section;
		}
	}
}