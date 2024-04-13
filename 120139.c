GF_Err hnti_Read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read_ex(s, bs, hnti_AddBox, s->type);
}