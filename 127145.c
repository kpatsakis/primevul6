R_API RList *r_bin_dwarf_parse_line(RBin *a, int mode) {
	ut8 *buf;
	RList *list = NULL;
	int len, ret;
	RBinSection *section = getsection (a, "debug_line");
	RBinFile *binfile = a ? a->cur: NULL;
	if (binfile && section) {
		len = section->size;
		if (len < 1) {
			return NULL;
		}
		buf = calloc (1, len + 1);
		if (!buf) {
			return NULL;
		}
		ret = r_buf_read_at (binfile->buf, section->paddr, buf, len);
		if (ret != len) {
			free (buf);
			return NULL;
		}
		list = r_list_new (); // always return empty list wtf
		if (!list) {
			free (buf);
			return NULL;
		}
		list->free = r_bin_dwarf_row_free;
		r_bin_dwarf_parse_line_raw2 (a, buf, len, mode);
		// k bin/cur/addrinfo/*
		SdbListIter *iter;
		SdbKv *kv;
		SdbList *ls = sdb_foreach_list (binfile->sdb_addrinfo, false);
		ls_foreach (ls, iter, kv) {
			if (!strncmp (kv->key, "0x", 2)) {
				ut64 addr;
				RBinDwarfRow *row;
				int line;
				char *file = strdup (kv->value);
				if (!file) {
					free (buf);
					ls_free (ls);
					r_list_free (list);
					return NULL;
				}
				char *tok = strchr (file, '|');
				if (tok) {
					*tok++ = 0;
					line = atoi (tok);
					addr = r_num_math (NULL, kv->key);
					row = r_bin_dwarf_row_new (addr, file, line, 0);
					r_list_append (list, row);
				}
				free (file);
			}
		}
		ls_free (ls);
		free (buf);
	}
	return list;
}