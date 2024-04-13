static void naludmx_del_param_list(GF_List *ps)
{
	if (!ps) return;
	while (gf_list_count(ps)) {
		GF_NALUFFParam *sl = gf_list_pop_back(ps);
		if (sl->data) gf_free(sl->data);
		gf_free(sl);
	}
	gf_list_del(ps);
}