static void r_bin_mdmp_free_pe64_bin(void *pe_bin_) {
	struct Pe64_r_bin_mdmp_pe_bin *pe_bin = pe_bin_;
	if (pe_bin) {
		sdb_free (pe_bin->bin->kv);
		Pe64_r_bin_pe_free (pe_bin->bin);
		R_FREE (pe_bin);
	}
}