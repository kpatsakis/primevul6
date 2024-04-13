GF_Err kind_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_KindBox *ptr = (GF_KindBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_data(bs, ptr->schemeURI, (u32) (strlen(ptr->schemeURI) + 1 ));
	if (ptr->value) {
		gf_bs_write_data(bs, ptr->value, (u32) (strlen(ptr->value) + 1) );
	}
	return GF_OK;
}