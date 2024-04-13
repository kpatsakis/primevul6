s32 gf_mv12_next_start_code(unsigned char *pbuffer, u32 buflen, u32 *optr, u32 *scode)
{
	u32 value;
	u32 offset;

	if (buflen < 4) return -1;
	for (offset = 0; offset < buflen - 3; offset++, pbuffer++) {
#ifdef GPAC_BIG_ENDIAN
		value = *(u32 *)pbuffer >> 8;
#else
		value = (pbuffer[0] << 16) | (pbuffer[1] << 8) | (pbuffer[2] << 0);
#endif

		if (value == MPEG12_START_CODE_PREFIX) {
			*optr = offset;
			*scode = (value << 8) | pbuffer[3];
			return 0;
		}
	}
	return -1;
}