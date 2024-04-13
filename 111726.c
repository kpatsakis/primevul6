static void process_constructors(RBinFile *bf, RList *ret, int bits) {
	RList *secs = sections (bf);
	RListIter *iter;
	RBinSection *sec;
	int i, type;
	r_list_foreach (secs, iter, sec) {
		type = -1;
		if (strstr (sec->name, "_mod_fini_func")) {
			type  = R_BIN_ENTRY_TYPE_FINI;
		} else if (strstr (sec->name, "_mod_init_func")) {
			type  = R_BIN_ENTRY_TYPE_INIT;
		}
		if (type != -1) {
			ut8 *buf = calloc (sec->size, 1);
			if (!buf) {
				continue;
			}
			int read = r_buf_read_at (bf->buf, sec->paddr, buf, sec->size);
			if (read < sec->size) {
				eprintf ("process_constructors: cannot process section %s\n", sec->name);
				continue;
			}
			if (bits == 32) {
				for (i = 0; i + 3 < sec->size; i += 4) {
					ut32 addr32 = r_read_le32 (buf + i);
					RBinAddr *ba = newEntry (sec->paddr + i, (ut64)addr32, type, bits);
					if (ba) {
						r_list_append (ret, ba);
					}
				}
			} else {
				for (i = 0; i + 7 < sec->size; i += 8) {
					ut64 addr64 = r_read_le64 (buf + i);
					RBinAddr *ba = newEntry (sec->paddr + i, addr64, type, bits);
					if (ba) {
						r_list_append (ret, ba);
					}
				}
			}
			free (buf);
		}
	}
	r_list_free (secs);
}