GF_Err hnti_Write(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_write_header(s, bs);
}