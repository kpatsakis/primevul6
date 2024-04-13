GF_Err def_cont_box_Read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read(s, bs, gf_isom_box_add_default);
}