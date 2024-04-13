static Bool adts_dmx_sync_frame_bs(GF_BitStream *bs, ADTSHeader *hdr)
{
	u32 val;
	u64 pos;

	while (gf_bs_available(bs)>7) {
		val = gf_bs_read_u8(bs);
		if (val!=0xFF) continue;
		val = gf_bs_read_int(bs, 4);
		if (val != 0x0F) {
			gf_bs_read_int(bs, 4);
			continue;
		}
		hdr->is_mp2 = (Bool)gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 2);
		hdr->no_crc = (Bool)gf_bs_read_int(bs, 1);
		pos = gf_bs_get_position(bs) - 2;

		hdr->profile = 1 + gf_bs_read_int(bs, 2);
		hdr->sr_idx = gf_bs_read_int(bs, 4);
		gf_bs_read_int(bs, 1);
		hdr->nb_ch = gf_bs_read_int(bs, 3);
		//value 1->6 match channel number, value 7 is 7.1
		if (hdr->nb_ch==7)
			hdr->nb_ch = 8;

		gf_bs_read_int(bs, 4);
		hdr->frame_size = gf_bs_read_int(bs, 13);
		gf_bs_read_int(bs, 11);
		gf_bs_read_int(bs, 2);
		hdr->hdr_size = 7;
		if (!hdr->no_crc) {
			gf_bs_read_u16(bs);
			hdr->hdr_size = 9;
		}
		if (!GF_M4ASampleRates[hdr->sr_idx] || (hdr->frame_size < hdr->hdr_size)) {
			gf_bs_seek(bs, pos+1);
			continue;
		}
		hdr->frame_size -= hdr->hdr_size;

		if (gf_bs_available(bs) == hdr->frame_size) {
			return GF_TRUE;
		}
		if (gf_bs_available(bs) < hdr->frame_size) {
			break;
		}

		gf_bs_skip_bytes(bs, hdr->frame_size);
		val = gf_bs_read_u8(bs);
		if (val!=0xFF) {
			gf_bs_seek(bs, pos+1);
			continue;
		}
		val = gf_bs_read_int(bs, 4);
		if (val!=0x0F) {
			gf_bs_read_int(bs, 4);
			gf_bs_seek(bs, pos+1);
			continue;
		}
		gf_bs_seek(bs, pos+hdr->hdr_size);
		return GF_TRUE;
	}
	return GF_FALSE;
}