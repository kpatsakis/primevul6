u32 gf_isom_probe_file_range(const char *fileName, u64 start_range, u64 end_range)
{
	u32 type = 0;

	if (!strncmp(fileName, "gmem://", 7)) {
		u32 size;
		u8 *mem_address;
		if (gf_blob_get(fileName, &mem_address, &size, NULL) != GF_OK) {
			return 0;
		}
        if (size && (size > start_range + 8)) {
			type = GF_4CC(mem_address[start_range + 4], mem_address[start_range + 5], mem_address[start_range + 6], mem_address[start_range + 7]);
        }
        gf_blob_release(fileName);
	} else if (!strncmp(fileName, "isobmff://", 10)) {
		return 2;
	} else {
		u32 nb_read;
		unsigned char data[4];
		FILE *f = gf_fopen(fileName, "rb");
		if (!f) return 0;
		if (start_range) gf_fseek(f, start_range, SEEK_SET);
		type = 0;
		nb_read = (u32) gf_fread(data, 4, f);
		if (nb_read == 4) {
			if (gf_fread(data, 4, f) == 4) {
				type = GF_4CC(data[0], data[1], data[2], data[3]);
			}
		}
		gf_fclose(f);
		if (!nb_read) return 0;
	}
	return gf_isom_probe_type(type);
}