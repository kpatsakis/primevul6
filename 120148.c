
GF_Err paen_Read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read(s, bs, fiin_AddBox);