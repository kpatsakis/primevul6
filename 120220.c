
GF_Err fdpa_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_FDpacketBox *ptr = (GF_FDpacketBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_int(bs, ptr->info.sender_current_time_present, 1);
	gf_bs_write_int(bs, ptr->info.expected_residual_time_present, 1);
	gf_bs_write_int(bs, ptr->info.session_close_bit, 1);
	gf_bs_write_int(bs, ptr->info.object_close_bit, 1);
	gf_bs_write_int(bs, 0, 4);
	ptr->info.transport_object_identifier = gf_bs_read_u16(bs);
	gf_bs_write_u16(bs, ptr->header_ext_count);
	for (i=0; i<ptr->header_ext_count; i++) {
		gf_bs_write_u8(bs, ptr->headers[i].header_extension_type);
		if (ptr->headers[i].header_extension_type > 127) {
			gf_bs_write_data(bs, (const char *) ptr->headers[i].content, 3);
		} else {
			gf_bs_write_u8(bs, ptr->headers[i].data_length ? (ptr->headers[i].data_length+2)/4 : 0);
			if (ptr->headers[i].data_length) {
				gf_bs_write_data(bs, ptr->headers[i].data, ptr->headers[i].data_length);
			}
		}
	}
	return GF_OK;