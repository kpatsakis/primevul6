
GF_Err trgr_AddBox(GF_Box *s, GF_Box *a)
{
	GF_TrackGroupBox *ptr = (GF_TrackGroupBox *)s;
	return gf_list_add(ptr->groups, a);