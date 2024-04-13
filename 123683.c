static const char *adts_dmx_probe_data(const u8 *data, u32 size, GF_FilterProbeScore *score)
{
	u32 nb_frames=0, next_pos=0, max_consecutive_frames=0;
	ADTSHeader prev_hdr;
	GF_BitStream *bs;
	Bool has_id3=GF_FALSE;
	Bool has_broken_data=GF_FALSE;

	/*check for id3*/
	if (size>= 10) {
		if (data[0] == 'I' && data[1] == 'D' && data[2] == '3') {
			u32 tag_size = ((data[9] & 0x7f) + ((data[8] & 0x7f) << 7) + ((data[7] & 0x7f) << 14) + ((data[6] & 0x7f) << 21));

			if (tag_size+10 > size) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("ID3 tag detected size %d but probe data only %d bytes, will rely on file extension (try increasing probe size using --block_size)\n", tag_size+10, size));
				*score = GF_FPROBE_EXT_MATCH;
				return "aac|adts";
			}
			data += tag_size+10;
			size -= tag_size+10;
			has_id3 = GF_TRUE;
		}
	}

	bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
	memset(&prev_hdr, 0, sizeof(ADTSHeader));
	while (gf_bs_available(bs)) {
		ADTSHeader hdr;
		u32 pos;
		hdr.frame_size = 0;
		if (!adts_dmx_sync_frame_bs(bs, &hdr)) {
			if (hdr.frame_size) {
				//nb_frames++;
				max_consecutive_frames++;
			}
			break;
		}
		if ((hdr.hdr_size!=7) && (hdr.hdr_size!=9)) continue;
//		if (!hdr.nb_ch) continue;
		pos = (u32) gf_bs_get_position(bs);

		if (!nb_frames) {
			nb_frames = 1;
		} else if ((next_pos + hdr.hdr_size == pos) && (hdr.sr_idx==prev_hdr.sr_idx) && (hdr.nb_ch==prev_hdr.nb_ch) ) {
			nb_frames++;
			if (max_consecutive_frames<nb_frames) max_consecutive_frames = nb_frames;
			if (max_consecutive_frames>5)
				break;
		} else {
			nb_frames=1;
			has_broken_data=GF_TRUE;
		}
		prev_hdr = hdr;
		gf_bs_skip_bytes(bs, hdr.frame_size);
		next_pos = (u32) gf_bs_get_position(bs);
	}
	gf_bs_del(bs);
	if (max_consecutive_frames>=4) {
		*score = has_broken_data ? GF_FPROBE_MAYBE_SUPPORTED : GF_FPROBE_SUPPORTED;
		return "audio/aac";
	}
	if (has_id3 && max_consecutive_frames) {
		*score = GF_FPROBE_MAYBE_SUPPORTED;
		return "audio/aac";
	}
	return NULL;
}