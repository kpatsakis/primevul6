R_API int r_bin_java_new_bin(RBinJavaObj *bin, ut64 loadaddr, Sdb *kv, const ut8 *buf, ut64 len) {
	R_BIN_JAVA_GLOBAL_BIN = bin;
	bin->lines.count = 0;
	bin->loadaddr = loadaddr;
	r_bin_java_get_java_null_cp ();
	bin->id = r_num_rand (UT32_MAX);
	bin->kv = kv ? kv : sdb_new (NULL, NULL, 0);
	bin->AllJavaBinObjs = NULL;
	return r_bin_java_load_bin (bin, buf, len);
}