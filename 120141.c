GF_Err mvex_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MovieExtendsBox *ptr = (GF_MovieExtendsBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	if (ptr->mehd) {
		e = gf_isom_box_write((GF_Box *)ptr->mehd, bs);
		if (e) return e;
	}
	e = gf_isom_box_array_write(s, ptr->TrackExList, bs);
	if (e) return e;
	return gf_isom_box_array_write(s, ptr->TrackExPropList, bs);
}