
GF_Err trgr_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TrackGroupBox *ptr = (GF_TrackGroupBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	return gf_isom_box_array_write(s, ptr->groups, bs);