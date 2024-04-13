GF_Err moof_Size(GF_Box *s)
{
	GF_Err e;
	GF_MovieFragmentBox *ptr = (GF_MovieFragmentBox *)s;

	if (ptr->mfhd) {
		e = gf_isom_box_size((GF_Box *)ptr->mfhd);
		if (e) return e;
		ptr->size += ptr->mfhd->size;
	}
	return gf_isom_box_array_size(s, ptr->TrackList);
}