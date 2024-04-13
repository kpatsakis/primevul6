void r_bin_mdmp_free(struct r_bin_mdmp_obj *obj) {
	if (!obj) return;

	r_list_free (obj->streams.ex_threads);
	r_list_free (obj->streams.memories);
	r_list_free (obj->streams.memories64.memories);
	r_list_free (obj->streams.memory_infos);
	r_list_free (obj->streams.modules);
	r_list_free (obj->streams.operations);
	r_list_free (obj->streams.thread_infos);
	r_list_free (obj->streams.threads);
	r_list_free (obj->streams.unloaded_modules);

	r_list_free (obj->pe32_bins);
	r_list_free (obj->pe64_bins);

	r_buf_free (obj->b);
	obj->b = NULL;
	free (obj);

	return;
}