static int r_cmd_java_resolve_cp_address (RBinJavaObj *obj, ut16 idx) {
	if (obj && idx){
		ut64 addr = r_bin_java_resolve_cp_idx_address (obj, idx) ;
		if (addr == -1) {
			r_cons_printf ("Unable to resolve CP Object @ index: 0x%04x\n", idx);
		} else {
			r_cons_printf ("0x%" PFMT64x "\n", addr);
		}
	}
	return true;
}