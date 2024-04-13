ut32 r_bin_mdmp_get_srwx(struct r_bin_mdmp_obj *obj, ut64 vaddr) {
	struct minidump_memory_info *mem_info;

	if (!(mem_info = r_bin_mdmp_get_mem_info(obj, vaddr))) {
		return 0;
	}

	/* FIXME: Have I got these mappings right, I am not sure I have!!! */

	switch (mem_info->protect) {
	case MINIDUMP_PAGE_READONLY:
		return R_BIN_SCN_READABLE;
	case MINIDUMP_PAGE_READWRITE:
		return R_BIN_SCN_READABLE | R_BIN_SCN_WRITABLE;
	case MINIDUMP_PAGE_EXECUTE:
		return R_BIN_SCN_EXECUTABLE;
	case MINIDUMP_PAGE_EXECUTE_READ:
		return R_BIN_SCN_EXECUTABLE | R_BIN_SCN_READABLE;
	case MINIDUMP_PAGE_EXECUTE_READWRITE:
		return R_BIN_SCN_EXECUTABLE | R_BIN_SCN_READABLE | R_BIN_SCN_WRITABLE;
	case MINIDUMP_PAGE_NOACCESS:
	case MINIDUMP_PAGE_WRITECOPY:
	case MINIDUMP_PAGE_EXECUTE_WRITECOPY:
	case MINIDUMP_PAGE_GUARD:
	case MINIDUMP_PAGE_NOCACHE:
	case MINIDUMP_PAGE_WRITECOMBINE:
	default:
		return 0;
	}
}