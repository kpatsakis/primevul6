
void trun_del(GF_Box *s)
{
	GF_TrunEntry *p;
	GF_TrackFragmentRunBox *ptr = (GF_TrackFragmentRunBox *)s;
	if (ptr == NULL) return;

	while (gf_list_count(ptr->entries)) {
		p = (GF_TrunEntry*)gf_list_get(ptr->entries, 0);
		gf_list_rem(ptr->entries, 0);
		gf_free(p);
	}
	gf_list_del(ptr->entries);
	if (ptr->cache) gf_bs_del(ptr->cache);
	gf_free(ptr);