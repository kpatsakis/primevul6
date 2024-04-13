GF_Err nmhd_Write(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_full_box_write(s, bs);
}