u16 gf_mp3_frame_size(u32 hdr)
{
	u8 version = gf_mp3_version(hdr);
	u8 layer = gf_mp3_layer(hdr);
	u32 pad = ((hdr >> 9) & 0x1) ? 1 : 0;
	u32 bitrate = gf_mp3_bit_rate(hdr);
	u32 samplerate = gf_mp3_sampling_rate(hdr);

	u32 frameSize = 0;
	if (!samplerate || !bitrate) return 0;

	if (layer == 1) {
		frameSize = ((12 * bitrate / samplerate) + pad) * 4;
	}
	else {
		u32 slots_per_frame = 144;
		if ((layer == 3) && !(version & 1)) slots_per_frame = 72;
		frameSize = (slots_per_frame * bitrate / samplerate) + pad;
	}
	return (u16)frameSize;
}