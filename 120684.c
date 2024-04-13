}
GF_Err fdsa_Read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read(s, bs, fdsa_AddBox);