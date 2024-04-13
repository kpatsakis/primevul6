u32 gf_mp3_get_next_header(FILE* in)
{
	u8 b, state = 0;
	u32 dropped = 0;
	unsigned char bytes[4];
	bytes[0] = bytes[1] = bytes[2] = bytes[3] = 0;

	while (1) {
		if (gf_fread(&b, 1, in) == 0) return 0;

		if (state == 3) {
			bytes[state] = b;
			return GF_4CC((u32)bytes[0], bytes[1], bytes[2], bytes[3]);
		}
		if (state == 2) {
			if (((b & 0xF0) == 0) || ((b & 0xF0) == 0xF0) || ((b & 0x0C) == 0x0C)) {
				if (bytes[1] == 0xFF) state = 1;
				else state = 0;
			}
			else {
				bytes[state] = b;
				state = 3;
			}
		}
		if (state == 1) {
			if (((b & 0xE0) == 0xE0) && ((b & 0x18) != 0x08) && ((b & 0x06) != 0)) {
				bytes[state] = b;
				state = 2;
			}
			else {
				state = 0;
			}
		}

		if (state == 0) {
			if (b == 0xFF) {
				bytes[state] = b;
				state = 1;
			}
			else {
				if ((dropped == 0) && ((b & 0xE0) == 0xE0) && ((b & 0x18) != 0x08) && ((b & 0x06) != 0)) {
					bytes[0] = (u8)0xFF;
					bytes[1] = b;
					state = 2;
				}
				else {
					dropped++;
				}
			}
		}
	}
	return 0;
}