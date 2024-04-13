GF_Err mfra_Size(GF_Box *s)
{
	GF_Err e;
	GF_MovieFragmentRandomAccessBox *ptr = (GF_MovieFragmentRandomAccessBox *)s;

	if (ptr->mfro) {
		e = gf_isom_box_size((GF_Box *)ptr->mfro);
		if (e) return e;
		ptr->size += ptr->mfro->size;
	}
	return gf_isom_box_array_size(s, ptr->tfra_list);
}