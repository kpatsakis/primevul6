static bool load_buffer(RBinFile *bf, void **bin_obj, RBuffer *buf, ut64 loadaddr, Sdb *sdb){
	r_return_val_if_fail (bf && bin_obj && buf, false);
	struct MACH0_(opts_t) opts;
	MACH0_(opts_set_default) (&opts, bf);
	struct MACH0_(obj_t) *res = MACH0_(new_buf) (buf, &opts);
	if (res) {
		if (res->chained_starts) {
			RIO *io = bf->rbin->iob.io;
			swizzle_io_read (res, io);
		}
		sdb_ns_set (sdb, "info", res->kv);
		*bin_obj = res;
		return true;
	}
	return false;
}