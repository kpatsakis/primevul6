
GF_Err tref_Read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read_ex(s, bs, gf_isom_box_add_default, s->type);