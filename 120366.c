
GF_Err trak_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TrackBox *ptr = (GF_TrackBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	if (ptr->Header) {
		e = gf_isom_box_write((GF_Box *) ptr->Header, bs);
		if (e) return e;
	}
	if (ptr->Aperture) {
		e = gf_isom_box_write((GF_Box *) ptr->Aperture, bs);
		if (e) return e;
	}
	if (ptr->References) {
		e = gf_isom_box_write((GF_Box *) ptr->References, bs);
		if (e) return e;
	}
	if (ptr->editBox) {
		e = gf_isom_box_write((GF_Box *) ptr->editBox, bs);
		if (e) return e;
	}
	if (ptr->Media) {
		e = gf_isom_box_write((GF_Box *) ptr->Media, bs);
		if (e) return e;
	}
	if (ptr->meta) {
		e = gf_isom_box_write((GF_Box *) ptr->meta, bs);
		if (e) return e;
	}
	if (ptr->groups) {
		e = gf_isom_box_write((GF_Box *) ptr->groups, bs);
		if (e) return e;
	}
	if (ptr->udta) {
		e = gf_isom_box_write((GF_Box *) ptr->udta, bs);
		if (e) return e;
	}
	return GF_OK;