GF_Err payt_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 length;
	GF_PAYTBox *ptr = (GF_PAYTBox *)s;

	ptr->payloadCode = gf_bs_read_u32(bs);
	length = gf_bs_read_u8(bs);
	ptr->payloadString = (char*)gf_malloc(sizeof(char) * (length+1) );
	if (! ptr->payloadString) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->payloadString, length);
	ptr->payloadString[length] = 0;

	ISOM_DECREASE_SIZE(ptr, (4+length+1) );
	return GF_OK;
}