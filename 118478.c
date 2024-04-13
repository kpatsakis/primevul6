Bool gf_m2ts_probe_file(const char *fileName)
{
	char buf[M2TS_PROBE_SIZE];
	u32 size;
	FILE *t;

	if (!strncmp(fileName, "gmem://", 7)) {
		u8 *mem_address;
		if (gf_blob_get_data(fileName, &mem_address, &size) != GF_OK) {
			return GF_FALSE;
		}
		if (size>M2TS_PROBE_SIZE) size = M2TS_PROBE_SIZE;
		memcpy(buf, mem_address, size);
	} else {
		t = gf_fopen(fileName, "rb");
		if (!t) return 0;
		size = (u32) fread(buf, 1, M2TS_PROBE_SIZE, t);
		gf_fclose(t);
		if ((s32) size <= 0) return 0;
	}
	return gf_m2ts_probe_buffer(buf, size);
}