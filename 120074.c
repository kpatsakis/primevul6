GF_Err mfra_Read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read(s, bs, mfra_AddBox);
}