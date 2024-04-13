s32 gf_mv12_next_slice_start(unsigned char *pbuffer, u32 startoffset, u32 buflen, u32 *slice_offset)
{
	u32 slicestart, code;
	while (gf_mv12_next_start_code(pbuffer + startoffset, buflen - startoffset, &slicestart, &code) >= 0) {
		if ((code >= MPEG12_SLICE_MIN_START) && (code <= MPEG12_SLICE_MAX_START)) {
			*slice_offset = slicestart + startoffset;
			return 0;
		}
		startoffset += slicestart + 4;
	}
	return -1;
}