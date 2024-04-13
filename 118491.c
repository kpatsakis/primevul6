GF_Err gf_m2ts_process_data(GF_M2TS_Demuxer *ts, u8 *data, u32 data_size)
{
	GF_Err e=GF_OK;
	u32 pos, pck_size;
	Bool is_align = 1;

	if (ts->buffer_size) {
		//we are sync, copy remaining bytes
		if ( (ts->buffer[0]==0x47) && (ts->buffer_size<200)) {
			u32 pck_size = ts->prefix_present ? 192 : 188;

			if (ts->alloc_size < 200) {
				ts->alloc_size = 200;
				ts->buffer = (char*)gf_realloc(ts->buffer, sizeof(char)*ts->alloc_size);
			}
			memcpy(ts->buffer + ts->buffer_size, data, pck_size - ts->buffer_size);
			e |= gf_m2ts_process_packet(ts, (unsigned char *)ts->buffer);
			data += (pck_size - ts->buffer_size);
			data_size = data_size - (pck_size - ts->buffer_size);
		}
		//not sync, copy over the complete buffer
		else {
			if (ts->alloc_size < ts->buffer_size+data_size) {
				ts->alloc_size = ts->buffer_size+data_size;
				ts->buffer = (char*)gf_realloc(ts->buffer, sizeof(char)*ts->alloc_size);
			}
			memcpy(ts->buffer + ts->buffer_size, data, sizeof(char)*data_size);
			ts->buffer_size += data_size;
			is_align = 0;
			data = ts->buffer;
			data_size = ts->buffer_size;
		}
	}

	/*sync input data*/
	pos = gf_m2ts_sync(ts, data, data_size, is_align);
	if (pos==data_size) {
		if (is_align) {
			if (ts->alloc_size<data_size) {
				ts->buffer = (char*)gf_realloc(ts->buffer, sizeof(char)*data_size);
				ts->alloc_size = data_size;
			}
			memcpy(ts->buffer, data, sizeof(char)*data_size);
			ts->buffer_size = data_size;
		}
		return GF_OK;
	}
	pck_size = ts->prefix_present ? 192 : 188;
	for (;;) {
		/*wait for a complete packet*/
		if (data_size < pos  + pck_size) {
			ts->buffer_size = data_size - pos;
			data += pos;
			if (!ts->buffer_size) {
				return e;
			}
			assert(ts->buffer_size<pck_size);

			if (is_align) {
				u32 s = ts->buffer_size;
				if (s<200) s = 200;

				if (ts->alloc_size < s) {
					ts->alloc_size = s;
					ts->buffer = (char*)gf_realloc(ts->buffer, sizeof(char)*ts->alloc_size);
				}
				memcpy(ts->buffer, data, sizeof(char)*ts->buffer_size);
			} else {
				memmove(ts->buffer, data, sizeof(char)*ts->buffer_size);
			}
			return e;
		}
		/*process*/
		e |= gf_m2ts_process_packet(ts, (unsigned char *)data + pos);
		pos += pck_size;
	}
	return e;
}