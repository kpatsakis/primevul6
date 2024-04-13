
GF_Err trgr_Size(GF_Box *s)
{
	GF_TrackGroupBox *ptr = (GF_TrackGroupBox *)s;
	return gf_isom_box_array_size(s, ptr->groups);