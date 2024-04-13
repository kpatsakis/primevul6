void elst_del(GF_Box *s)
{
	GF_EditListBox *ptr;
	GF_EdtsEntry *p;
	u32 nb_entries;
	u32 i;

	ptr = (GF_EditListBox *)s;
	if (ptr == NULL) return;
	nb_entries = gf_list_count(ptr->entryList);
	for (i = 0; i < nb_entries; i++) {
		p = (GF_EdtsEntry*)gf_list_get(ptr->entryList, i);
		if (p) gf_free(p);
	}
	gf_list_del(ptr->entryList);
	gf_free(ptr);
}