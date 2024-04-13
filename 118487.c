void gf_m2ts_pes_header(GF_M2TS_PES *pes, unsigned char *data, u32 data_size, GF_M2TS_PESHeader *pesh)
{
	u32 has_pts, has_dts;
	u32 len_check;
	memset(pesh, 0, sizeof(GF_M2TS_PESHeader));

	len_check = 0;

	pesh->id = data[0];
	pesh->pck_len = (data[1]<<8) | data[2];
	/*
		2bits
		scrambling_control		= gf_bs_read_int(bs,2);
		priority				= gf_bs_read_int(bs,1);
	*/
	pesh->data_alignment = (data[3] & 0x4) ? 1 : 0;
	/*
		copyright				= gf_bs_read_int(bs,1);
		original				= gf_bs_read_int(bs,1);
	*/
	has_pts = (data[4]&0x80);
	has_dts = has_pts ? (data[4]&0x40) : 0;
	/*
		ESCR_flag				= gf_bs_read_int(bs,1);
		ES_rate_flag			= gf_bs_read_int(bs,1);
		DSM_flag				= gf_bs_read_int(bs,1);
		additional_copy_flag	= gf_bs_read_int(bs,1);
		prev_crc_flag			= gf_bs_read_int(bs,1);
		extension_flag			= gf_bs_read_int(bs,1);
	*/

	pesh->hdr_data_len = data[5];

	data += 6;
	if (has_pts) {
		pesh->PTS = gf_m2ts_get_pts(data);
		data+=5;
		len_check += 5;
	}
	if (has_dts) {
		pesh->DTS = gf_m2ts_get_pts(data);
		//data+=5;
		len_check += 5;
	} else {
		pesh->DTS = pesh->PTS;
	}
	if (len_check < pesh->hdr_data_len) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d Skipping %d bytes in pes header\n", pes->pid, pesh->hdr_data_len - len_check));
	} else if (len_check > pesh->hdr_data_len) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d Wrong pes_header_data_length field %d bytes - read %d\n", pes->pid, pesh->hdr_data_len, len_check));
	}

	if ((pesh->PTS<90000) && ((s32)pesh->DTS<0)) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] PID %d Wrong DTS %d negative for PTS %d - forcing to 0\n", pes->pid, pesh->DTS, pesh->PTS));
		pesh->DTS=0;
	}
}