
GF_Err trgr_Read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read_ex(s, bs, trgr_AddBox, s->type);