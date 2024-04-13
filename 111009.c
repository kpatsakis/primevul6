static u32 AC3_FindSyncCode(u8 *buf, u32 buflen)
{
	u32 end = buflen - 6;
	u32 offset = 0;
	while (offset <= end) {
		if (buf[offset] == 0x0b && buf[offset + 1] == 0x77) {
			return offset;
		}
		offset++;
	}
	return buflen;
}