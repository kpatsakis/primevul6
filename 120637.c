GF_Err stsd_Read(GF_Box *s, GF_BitStream *bs)
{
	gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(s, 4)

	return gf_isom_box_array_read_ex(s, bs, stsd_AddBox, GF_ISOM_BOX_TYPE_STSD);
}