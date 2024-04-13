GF_Err dinf_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_DataInformationBox *ptr = (GF_DataInformationBox *)s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	if (ptr->dref) {
		e = gf_isom_box_write((GF_Box *)ptr->dref, bs);
		if (e) return e;
	}
	return GF_OK;
}