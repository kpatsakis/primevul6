void mfra_del(GF_Box *s)
{
	GF_MovieFragmentRandomAccessBox *ptr = (GF_MovieFragmentRandomAccessBox *)s;
	if (ptr == NULL) return;
	if (ptr->mfro) gf_isom_box_del((GF_Box*)ptr->mfro);
	gf_isom_box_array_del(ptr->tfra_list);
	gf_free(ptr);
}