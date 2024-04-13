u32 gf_media_nalu_emulation_bytes_remove_count(const u8 *buffer, u32 nal_size)
{
	u32 i = 0, emulation_bytes_count = 0;
	u8 num_zero = 0;
	if (!buffer || !nal_size) return 0;

	while (i < nal_size)
	{
		/*ISO 14496-10: "Within the NAL unit, any four-byte sequence that starts with 0x000003
		  other than the following sequences shall not occur at any byte-aligned position:
		  \96 0x00000300
		  \96 0x00000301
		  \96 0x00000302
		  \96 0x00000303"
		*/
		if (num_zero == 2
			&& buffer[i] == 0x03
			&& i + 1 < nal_size /*next byte is readable*/
			&& (u8)buffer[i + 1] < 0x04)
		{
			/*emulation code found*/
			num_zero = 0;
			emulation_bytes_count++;
			i++;
		}

		if (!buffer[i])
			num_zero++;
		else
			num_zero = 0;

		i++;
	}

	return emulation_bytes_count;
}