
GF_Err tref_Write(GF_Box *s, GF_BitStream *bs)
{
//	GF_TrackReferenceBox *ptr = (GF_TrackReferenceBox *)s;
	return gf_isom_box_write_header(s, bs);