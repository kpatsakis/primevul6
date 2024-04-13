R_API RBinJavaObj *r_bin_java_new_buf(RBuffer *buf, ut64 loadaddr, Sdb *kv) {
	RBinJavaObj *bin = R_NEW0 (RBinJavaObj);
	if (!bin) {
		return NULL;
	}
	if (!r_bin_java_new_bin (bin, loadaddr, kv, buf->buf, buf->length)) {
		return r_bin_java_free (bin);
	}
	return bin;
}