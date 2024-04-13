GF_Err mfra_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MovieFragmentRandomAccessBox *ptr = (GF_MovieFragmentRandomAccessBox *)s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	e = gf_isom_box_array_write(s, ptr->tfra_list, bs);
	if (e) return e;
	if (ptr->mfro) {
		e = gf_isom_box_write((GF_Box *) ptr->mfro, bs);
		if (e) return e;
	}
	return GF_OK;
}