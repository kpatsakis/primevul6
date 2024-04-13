
GF_Err strk_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SubTrackBox *ptr = (GF_SubTrackBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	if (ptr->info) {
		e = gf_isom_box_write((GF_Box *)ptr->info, bs);
		if (e) return e;
	}
	return GF_OK;