u32 gf_opus_check_frame(GF_OpusParser *op, u8 *data, u32 data_length)
{
	u32 block_size;

	if (!memcmp(data, "OpusHead", sizeof(char)*8))
		return 0;
	if (!memcmp(data, "OpusTags", sizeof(char)*8))
		return 0;

	/*consider the whole packet as Ogg packets and ISOBMFF samples for Opus are framed similarly*/
	static const int OpusFrameDurIn48k[] = { 480, 960, 1920, 2880, 480, 960, 1920, 2880, 480, 960, 1920, 2880,
		480, 960, 480, 960,
		120, 240, 480, 960, 120, 240, 480, 960, 120, 240, 480, 960, 120, 240, 480, 960,
	};
	int TOC_config = (data[0] & 0xf8) >> 3;
	//int s = (data[0] & 0x04) >> 2;
	block_size = OpusFrameDurIn48k[TOC_config];

	int c = data[0] & 0x03;
	if (c == 1 || c == 2) {
		block_size *= 2;
	} else if (c == 3) {
		/*unknown number of frames*/
		int num_frames = data[1] & 0x3f;
		block_size *= num_frames;
	}
	return block_size;
}