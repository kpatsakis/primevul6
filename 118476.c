static GF_Err id3_parse_tag(char *data, u32 length, char **output, u32 *output_size, u32 *output_pos)
{
	GF_BitStream *bs;
	u32 pos;

	if ((data[0] != 'I') || (data[1] != 'D') || (data[2] != '3'))
		return GF_NOT_SUPPORTED;

	bs = gf_bs_new(data, length, GF_BITSTREAM_READ);

	gf_bs_skip_bytes(bs, 3);
	/*u8 major = */gf_bs_read_u8(bs);
	/*u8 minor = */gf_bs_read_u8(bs);
	/*u8 unsync = */gf_bs_read_int(bs, 1);
	/*u8 ext_hdr = */ gf_bs_read_int(bs, 1);
	gf_bs_read_int(bs, 6);
	u32 size = gf_id3_read_size(bs);

	pos = (u32) gf_bs_get_position(bs);
	if (size != length-pos)
		size = length-pos;

	while (size && (gf_bs_available(bs)>=10) ) {
		u32 ftag = gf_bs_read_u32(bs);
		u32 fsize = gf_id3_read_size(bs);
		/*u16 fflags = */gf_bs_read_u16(bs);
		size -= 10;

		//TODO, handle more ID3 tags ?
		if (ftag==ID3V2_FRAME_TXXX) {
			u32 pos = (u32) gf_bs_get_position(bs);
			char *text = data+pos;
			add_text(output, output_size, output_pos, text, fsize);
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[MPEG-2 TS] ID3 tag not handled, patch welcome\n", gf_4cc_to_str(ftag) ) );
		}
		gf_bs_skip_bytes(bs, fsize);
	}
	gf_bs_del(bs);
	return GF_OK;
}