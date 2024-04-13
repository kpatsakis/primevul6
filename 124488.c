static bool r_bin_mdmp_init_pe_bins(struct r_bin_mdmp_obj *obj) {
	bool dup;
	ut64 paddr;
	struct minidump_module *module;
	struct Pe32_r_bin_mdmp_pe_bin *pe32_bin, *pe32_dup;
	struct Pe64_r_bin_mdmp_pe_bin *pe64_bin, *pe64_dup;
	RBuffer *buf;
	RListIter *it, *it_dup;

	r_list_foreach (obj->streams.modules, it, module) {
		/* Duplicate modules can appear in the MDMP module list,
		** filtering them out seems to be the correct behaviour */
		if (!(paddr = r_bin_mdmp_get_paddr (obj, module->base_of_image))) {
			continue;
		}
		int left = 0;
		const ut8 *b = r_buf_get_at (obj->b, paddr, &left);
		buf = r_buf_new_with_bytes (b, R_MIN (left, module->size_of_image));
		dup = false;
		if (check_pe32_bytes (buf->buf, module->size_of_image)) {
			r_list_foreach(obj->pe32_bins, it_dup, pe32_dup) {
				if (pe32_dup->vaddr == module->base_of_image) {
					dup = true;
					continue;
				}
			}
			if (dup) {
				continue;
			}
			if (!(pe32_bin = R_NEW0 (struct Pe32_r_bin_mdmp_pe_bin))) {
				continue;
			}
			r_bin_mdmp_patch_pe_headers (buf);
			pe32_bin->vaddr = module->base_of_image;
			pe32_bin->paddr = paddr;
			pe32_bin->bin = Pe32_r_bin_pe_new_buf (buf, 0);

			r_list_append (obj->pe32_bins, pe32_bin);
		} else if (check_pe64_bytes (buf->buf, module->size_of_image)) {
			r_list_foreach(obj->pe64_bins, it_dup, pe64_dup) {
				if (pe64_dup->vaddr == module->base_of_image) {
					dup = true;
					continue;
				}
			}
			if (dup) {
				continue;
			}
			if (!(pe64_bin = R_NEW0 (struct Pe64_r_bin_mdmp_pe_bin))) {
				continue;
			}
			r_bin_mdmp_patch_pe_headers (buf);
			pe64_bin->vaddr = module->base_of_image;
			pe64_bin->paddr = paddr;
			pe64_bin->bin = Pe64_r_bin_pe_new_buf (buf, 0);

			r_list_append (obj->pe64_bins, pe64_bin);
		}
		r_buf_free (buf);
	}
	return true;
}