static RList *symbols(RBinFile *bf) {
	struct MACH0_(obj_t) *bin;
	int i;
	const struct symbol_t *syms = NULL;
	RBinSymbol *ptr = NULL;
	RBinObject *obj = bf? bf->o: NULL;
	RList *ret = r_list_newf (free);
#if 0
	const char *lang = "c"; // XXX deprecate this
#endif
	int wordsize = 0;
	if (!ret) {
		return NULL;
	}
	if (!obj || !obj->bin_obj) {
		free (ret);
		return NULL;
	}
	bool isStripped = false;
	wordsize = MACH0_(get_bits) (obj->bin_obj);

	// OLD CODE
	if (!(syms = MACH0_(get_symbols) (obj->bin_obj))) {
		return ret;
	}
	Sdb *symcache = sdb_new0 ();
	bin = (struct MACH0_(obj_t) *) obj->bin_obj;
	for (i = 0; !syms[i].last; i++) {
		if (syms[i].name == NULL || syms[i].name[0] == '\0' || syms[i].addr < 100) {
			continue;
		}
		if (!(ptr = R_NEW0 (RBinSymbol))) {
			break;
		}
		ptr->name = strdup ((char*)syms[i].name);
		ptr->is_imported = syms[i].is_imported;
		if (ptr->name[0] == '_' && !ptr->is_imported) {
			char *dn = r_bin_demangle (bf, ptr->name, ptr->name, ptr->vaddr, false);
			if (dn) {
				ptr->dname = dn;
				char *p = strchr (dn, '.');
				if (p) {
					if (IS_UPPER (ptr->name[0])) {
						ptr->classname = strdup (ptr->name);
						ptr->classname[p - ptr->name] = 0;
					} else if (IS_UPPER (p[1])) {
						ptr->classname = strdup (p + 1);
						p = strchr (ptr->classname, '.');
						if (p) {
							*p = 0;
						}
					}
				}
			}
		}
		ptr->forwarder = "NONE";
		ptr->bind = (syms[i].type == R_BIN_MACH0_SYMBOL_TYPE_LOCAL)? R_BIN_BIND_LOCAL_STR: R_BIN_BIND_GLOBAL_STR;
		ptr->type = R_BIN_TYPE_FUNC_STR;
		ptr->vaddr = syms[i].addr;
		ptr->paddr = syms[i].offset + obj->boffset;
		ptr->size = syms[i].size;
		ptr->bits = syms[i].bits;
		if (bin->hdr.cputype == CPU_TYPE_ARM && wordsize < 64) {
			_handle_arm_thumb (bin, &ptr);
		}
		ptr->ordinal = i;
		bin->dbg_info = strncmp (ptr->name, "radr://", 7)? 0: 1;
		sdb_set (symcache, sdb_fmt ("sym0x%"PFMT64x, ptr->vaddr), "found", 0);
#if 0
		if (!strncmp (ptr->name, "__Z", 3)) {
			lang = "c++";
		}
		if (!strncmp (ptr->name, "type.", 5)) {
			lang = "go";
		} else if (!strcmp (ptr->name, "_rust_oom")) {
			lang = "rust";
		}
#endif
		r_list_append (ret, ptr);
	}
	//functions from LC_FUNCTION_STARTS
	if (bin->func_start) {
		char symstr[128];
		ut64 value = 0, address = 0;
		const ut8 *temp = bin->func_start;
		const ut8 *temp_end = bin->func_start + bin->func_size;
		strcpy (symstr, "sym0x");
		while (temp + 3 < temp_end && *temp) {
			temp = r_uleb128_decode (temp, NULL, &value);
			address += value;
			ptr = R_NEW0 (RBinSymbol);
			if (!ptr) {
				break;
			}
			ptr->vaddr = bin->baddr + address;
			ptr->paddr = address;
			ptr->size = 0;
			ptr->name = r_str_newf ("func.%08"PFMT64x, ptr->vaddr);
			ptr->type = R_BIN_TYPE_FUNC_STR;
			ptr->forwarder = "NONE";
			ptr->bind = R_BIN_BIND_LOCAL_STR;
			ptr->ordinal = i++;
			if (bin->hdr.cputype == CPU_TYPE_ARM && wordsize < 64) {
				_handle_arm_thumb (bin, &ptr);
			}
			r_list_append (ret, ptr);
			// if any func is not found in syms then we can consider it is stripped
			if (!isStripped) {
				snprintf (symstr + 5, sizeof (symstr) - 5 , "%" PFMT64x, ptr->vaddr);
				if (!sdb_const_get (symcache, symstr, 0)) {
					isStripped = true;
				}
			}
		}
	}
#if 0
// this must be done in bobj.c not here
	if (bin->has_blocks_ext) {
		lang = !strcmp (lang, "c++") ? "c++ blocks ext." : "c blocks ext.";
	}
	bin->lang = lang;
#endif
	if (isStripped) {
		bin->dbg_info |= R_BIN_DBG_STRIPPED;
	}
	sdb_free (symcache);
	return ret;
}