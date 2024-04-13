
void trgr_del(GF_Box *s)
{
	GF_TrackGroupBox *ptr = (GF_TrackGroupBox *)s;
	if (ptr == NULL) return;
	gf_isom_box_array_del(ptr->groups);
	gf_free(ptr);