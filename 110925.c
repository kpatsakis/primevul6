Bool gf_media_aom_probe_annexb(GF_BitStream *bs)
{
	Bool res = GF_TRUE;
	u64 init_pos = gf_bs_get_position(bs);
	u64 sz = gf_av1_leb128_read(bs, NULL);
	if (!sz) res = GF_FALSE;
	while (sz > 0) {
		u8 Leb128Bytes = 0;
		u64 frame_unit_size = gf_av1_leb128_read(bs, &Leb128Bytes);

		if (!frame_unit_size) {
			res = GF_FALSE;
			break;
		}

		if (sz < Leb128Bytes + frame_unit_size) {
			res = GF_FALSE;
			break;
		}
		sz -= Leb128Bytes + frame_unit_size;

		while (frame_unit_size > 0) {
			ObuType obu_type;
			u64 pos, obu_length = gf_av1_leb128_read(bs, &Leb128Bytes);
			if (frame_unit_size < Leb128Bytes + obu_length) {
				res = GF_FALSE;
				break;
			}
			pos = gf_bs_get_position(bs);
			frame_unit_size -= Leb128Bytes;

			u8 tid, sid;
			Bool extflag, has_size;
			GF_Err e = gf_av1_parse_obu_header(bs, &obu_type, &extflag, &has_size, &tid, &sid);
			if (e) {
				res = GF_FALSE;
				break;
			}

			if (has_size) {
				obu_length = (u32)gf_av1_leb128_read(bs, NULL);
			}
			else {
				if (obu_length >= 1 + extflag) {
					obu_length = obu_length - 1 - extflag;
				}
				else {
					res = GF_FALSE;
					break;
				}
			}
			u32 hdr_size = (u32)(gf_bs_get_position(bs) - pos);
			obu_length += hdr_size;

			if (frame_unit_size < obu_length) {
				res = GF_FALSE;
				break;
			}
			frame_unit_size -= obu_length;
			gf_bs_skip_bytes(bs, obu_length - hdr_size);
		}
		if (!res) break;
	}
	gf_bs_seek(bs, init_pos);
	return res;
}