void moof_del(GF_Box *s)
{
	GF_MovieFragmentBox *ptr = (GF_MovieFragmentBox *)s;
	if (ptr == NULL) return;

	if (ptr->mfhd) gf_isom_box_del((GF_Box *) ptr->mfhd);
	gf_isom_box_array_del(ptr->TrackList);
	if (ptr->mdat) gf_free(ptr->mdat);
	gf_free(ptr);
}