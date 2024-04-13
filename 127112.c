static inline void add_sdb_addrline(Sdb *s, ut64 addr, const char *file, ut64 line, FILE *f, int mode) {
	const char *p;
	char *fileline;
	char offset[64];
	char *offset_ptr;

	if (!s || !file)
		return;
	p = r_str_rchr (file, NULL, '/');
	if (p) {
		p++;
	} else {
		p = file;
	}
	// includedirs and properly check full paths
	switch (mode) {
	case 1:
	case 'r':
	case '*':
		if (!f) {
			f = stdout;
		}
		fprintf (f, "CL %s:%d 0x%08"PFMT64x"\n", p, (int)line, addr);
		break;
	}
#if 0
	/* THIS IS TOO SLOW */
	if (r_file_exists (file)) {
		p = file;
	}
#else
	p = file;
#endif
	fileline = r_str_newf ("%s|%"PFMT64d, p, line);
	offset_ptr = sdb_itoa (addr, offset, 16);
	sdb_add (s, offset_ptr, fileline, 0);
	sdb_add (s, fileline, offset_ptr, 0);
	free (fileline);
}