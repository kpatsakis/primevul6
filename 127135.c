R_API int r_bin_dwarf_parse_aranges_raw(const ut8 *obuf, int len, FILE *f) {
	ut32 length, offset;
	ut16 version;
	ut32 debug_info_offset;
	ut8 address_size, segment_size;
	const ut8 *buf = obuf;
	int idx = 0;

	if (!buf || len< 4) {
		return false;
	}

	READ_BUF (length, ut32);
	if (f) {
		printf("parse_aranges\n");
		printf("length 0x%x\n", length);
	}

	if (idx+12>=len)
		return false;

	READ_BUF (version, ut16);
	if (f) {
		printf("Version %d\n", version);
	}

	READ_BUF (debug_info_offset, ut32);
	if (f) {
		fprintf (f, "Debug info offset %d\n", debug_info_offset);
	}

	READ_BUF (address_size, ut8);
	if (f) {
		fprintf (f, "address size %d\n", (int)address_size);
	}

	READ_BUF (segment_size, ut8);
	if (f) {
		fprintf (f, "segment size %d\n", (int)segment_size);
	}

	offset = segment_size + address_size * 2;

	if (offset) {
		ut64 n = (((ut64) (size_t)buf / offset) + 1) * offset - ((ut64)(size_t)buf);
		if (idx+n>=len) {
			return false;
		}
		buf += n;
		idx += n;
	}

	while ((buf - obuf) < len) {
		ut64 adr, length;
		if ((idx+8)>=len) {
			break;
		}
		READ_BUF (adr, ut64);
		READ_BUF (length, ut64);
		if (f) printf("length 0x%"PFMT64x" address 0x%"PFMT64x"\n", length, adr);
	}

	return 0;
}