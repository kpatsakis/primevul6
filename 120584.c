GF_Err mdia_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MediaBox *ptr = (GF_MediaBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	//Header first
	if (ptr->mediaHeader) {
		e = gf_isom_box_write((GF_Box *) ptr->mediaHeader, bs);
		if (e) return e;
	}
	//then handler
	if (ptr->handler) {
		e = gf_isom_box_write((GF_Box *) ptr->handler, bs);
		if (e) return e;
	}
	if (ptr->information) {
		e = gf_isom_box_write((GF_Box *) ptr->information, bs);
		if (e) return e;
	}
	return GF_OK;
}