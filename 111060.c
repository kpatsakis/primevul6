u32 gf_media_nalu_add_emulation_bytes(const u8 *buffer_src, u8 *buffer_dst, u32 nal_size)
{
	u32 i = 0, emulation_bytes_count = 0;
	u8 num_zero = 0;

	while (i < nal_size) {
		/*ISO 14496-10: "Within the NAL unit, any four-byte sequence that starts with 0x000003
		other than the following sequences shall not occur at any byte-aligned position:
		0x00000300
		0x00000301
		0x00000302
		0x00000303"
		*/
		if (num_zero == 2 && (u8)buffer_src[i] < 0x04) {
			/*add emulation code*/
			num_zero = 0;
			buffer_dst[i + emulation_bytes_count] = 0x03;
			emulation_bytes_count++;
			if (!buffer_src[i])
				num_zero = 1;
		}
		else {
			if (!buffer_src[i])
				num_zero++;
			else
				num_zero = 0;
		}
		buffer_dst[i + emulation_bytes_count] = buffer_src[i];
		i++;
	}
	return nal_size + emulation_bytes_count;
}