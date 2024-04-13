static const ut8 *r_bin_dwarf_parse_lnp_header (
		RBinFile *bf, const ut8 *buf, const ut8 *buf_end,
		RBinDwarfLNPHeader *hdr, FILE *f, int mode) {
	int i;
	Sdb *s;
	size_t count;
	const ut8 *tmp_buf = NULL;

	if (!hdr || !bf || !buf) return NULL;

	hdr->unit_length.part1 = READ (buf, ut32);
	if (hdr->unit_length.part1 == DWARF_INIT_LEN_64) {
		hdr->unit_length.part2 = READ (buf, ut32);
	}

	s = sdb_new (NULL, NULL, 0);

	hdr->version = READ (buf, ut16);

	if (hdr->unit_length.part1 == DWARF_INIT_LEN_64) {
		hdr->header_length = READ (buf, ut64);
	} else {
		hdr->header_length = READ (buf, ut32);
	}

	if (buf_end-buf < 8) {
		sdb_free (s);
		return NULL;
	}
	hdr->min_inst_len = READ (buf, ut8);
	//hdr->max_ops_per_inst = READ (buf, ut8);
	hdr->file_names = NULL;
	hdr->default_is_stmt = READ (buf, ut8);
	hdr->line_base = READ (buf, char);
	hdr->line_range = READ (buf, ut8);
	hdr->opcode_base = READ (buf, ut8);

	if (f) {
		fprintf (f, "DWARF LINE HEADER\n");
		fprintf (f, "  total_length: %d\n", hdr->unit_length.part1);
		fprintf (f, "  version: %d\n", hdr->version);
		fprintf (f, "  header_length: : %"PFMT64d"\n", hdr->header_length);
		fprintf (f, "  mininstlen: %d\n", hdr->min_inst_len);
		fprintf (f, "  is_stmt: %d\n", hdr->default_is_stmt);
		fprintf (f, "  line_base: %d\n", hdr->line_base);
		fprintf (f, "  line_range: %d\n", hdr->line_range);
		fprintf (f, "  opcode_base: %d\n", hdr->opcode_base);
	}

	if (hdr->opcode_base>0) {
		hdr->std_opcode_lengths = calloc(sizeof(ut8), hdr->opcode_base);

		for (i = 1; i <= hdr->opcode_base - 1; i++) {
			if (buf+2>buf_end) break;
			hdr->std_opcode_lengths[i] = READ (buf, ut8);
			if (f) {
				fprintf (f, " op %d %d\n", i, hdr->std_opcode_lengths[i]);
			}
		}
	} else {
		hdr->std_opcode_lengths = NULL;
	}

	i = 0;
	while (buf+1 < buf_end) {
		int maxlen = R_MIN ((size_t)(buf_end-buf)-1, 0xfff);
		int len = r_str_nlen ((const char*)buf, maxlen);
		char *str = r_str_ndup ((const char *)buf, len);
		if (len<1 || len >= 0xfff) {
			buf += 1;
			free (str);
			break;
		}
		if (*str != '/' && *str != '.') {
			// no more paths in here
			free (str);
			break;
		}
		if (f) {
			fprintf (f, "INCLUDEDIR (%s)\n", str);
		}
		add_sdb_include_dir (s, str, i);
		free (str);
		i++;
		buf += len + 1;
	}

	tmp_buf = buf;
	count = 0;
	for (i = 0; i < 2; i++) {
		while (buf+1<buf_end) {
			const char *filename = (const char *)buf;
			int maxlen = R_MIN ((size_t)(buf_end-buf-1), 0xfff);
			ut64 id_idx, mod_time, file_len;
			size_t namelen, len = r_str_nlen (filename, maxlen);

			if (!len) {
				buf++;
				break;
			}
			buf += len + 1;
			if (buf>=buf_end) { buf = NULL; goto beach; }
			buf = r_uleb128 (buf, buf_end-buf, &id_idx);
			if (buf>=buf_end) { buf = NULL; goto beach; }
			buf = r_uleb128 (buf, buf_end-buf, &mod_time);
			if (buf>=buf_end) { buf = NULL; goto beach; }
			buf = r_uleb128 (buf, buf_end-buf, &file_len);
			if (buf>=buf_end) { buf = NULL; goto beach; }

			if (i) {
				char *include_dir = NULL, *comp_dir = NULL;
				char *allocated_id = NULL;
				if (id_idx > 0) {
					include_dir = sdb_array_get (s, "includedirs", id_idx - 1, 0);
					if (include_dir && include_dir[0] != '/') {
						comp_dir = sdb_get (bf->sdb_addrinfo, "DW_AT_comp_dir", 0);
						if (comp_dir) {
							allocated_id = calloc (1, strlen (comp_dir) +
									strlen (include_dir) + 8);
							snprintf (allocated_id, strlen (comp_dir) + strlen (include_dir) + 8,
									"%s/%s/", comp_dir, include_dir);
							include_dir = allocated_id;
						}
					}
				} else {
					include_dir = sdb_get (bf->sdb_addrinfo, "DW_AT_comp_dir", 0);
					if (!include_dir)
						include_dir = "./";
				}

				namelen = len + (include_dir?strlen (include_dir):0) + 8;

				if (hdr->file_names) {
					hdr->file_names[count].name = calloc (sizeof(char), namelen);
					snprintf (hdr->file_names[count].name, namelen - 1,
						"%s/%s", include_dir? include_dir : "", filename);
					hdr->file_names[count].name[namelen - 1] = '\0';
					free (allocated_id);
					hdr->file_names[count].id_idx = id_idx;
					hdr->file_names[count].mod_time = mod_time;
					hdr->file_names[count].file_len = file_len;
				}
			}
			count++;
			if (f && i) {
				fprintf (f, "FILE (%s)\n", filename);
				fprintf (f, "| dir idx %"PFMT64d"\n", id_idx);
				fprintf (f, "| lastmod %"PFMT64d"\n", mod_time);
				fprintf (f, "| filelen %"PFMT64d"\n", file_len);
			}
		}
		if (i == 0) {
			if (count>0) {
				hdr->file_names = calloc(sizeof(file_entry), count);
			} else {
				hdr->file_names = NULL;
			}
			hdr->file_names_count = count;
			buf = tmp_buf;
			count = 0;
		}
	}

beach:
	sdb_free (s);

	return buf;
}