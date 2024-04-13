static int r_bin_mdmp_init(struct r_bin_mdmp_obj *obj) {
	r_bin_mdmp_init_parsing (obj);

	if (!r_bin_mdmp_init_hdr (obj)) {
		eprintf ("[ERROR] Failed to initialise header\n");
		return false;
	}

	if (!r_bin_mdmp_init_directory (obj)) {
		eprintf ("[ERROR] Failed to initialise directory structures!\n");
		return false;
	}

	if (!r_bin_mdmp_init_pe_bins (obj)) {
		eprintf ("[ERROR] Failed to initialise pe binaries!\n");
		return false;
	}

	return true;
}