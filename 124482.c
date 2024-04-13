struct r_bin_mdmp_obj *r_bin_mdmp_new_buf(struct r_buf_t *buf) {
	bool fail = false;
	struct r_bin_mdmp_obj *obj = R_NEW0 (struct r_bin_mdmp_obj);
	if (!obj) {
		return NULL;
	}
	obj->kv = sdb_new0 ();
	obj->b = r_buf_new ();
	obj->size = (ut32)buf->length;

	fail |= (!(obj->streams.ex_threads = r_list_new ()));
	fail |= (!(obj->streams.memories = r_list_new ()));
	fail |= (!(obj->streams.memories64.memories = r_list_new ()));
	fail |= (!(obj->streams.memory_infos = r_list_new ()));
	fail |= (!(obj->streams.modules = r_list_new ()));
	fail |= (!(obj->streams.operations = r_list_new ()));
	fail |= (!(obj->streams.thread_infos = r_list_new ()));
	fail |= (!(obj->streams.threads = r_list_new ()));
	fail |= (!(obj->streams.unloaded_modules = r_list_new ()));

	fail |= (!(obj->pe32_bins = r_list_newf (r_bin_mdmp_free_pe32_bin)));
	fail |= (!(obj->pe64_bins = r_list_newf (r_bin_mdmp_free_pe64_bin)));

	if (fail) {
		r_bin_mdmp_free (obj);
		return NULL;
	}

	if (!r_buf_set_bytes (obj->b, buf->buf, buf->length)) {
		r_bin_mdmp_free (obj);
		return NULL;
	}

	if (!r_bin_mdmp_init (obj)) {
		r_bin_mdmp_free (obj);
		return NULL;
	}

	return obj;
}