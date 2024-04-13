
GF_Err trak_Size(GF_Box *s)
{
	GF_Err e;
	GF_TrackBox *ptr = (GF_TrackBox *)s;

	if (ptr->Header) {
		e = gf_isom_box_size((GF_Box *) ptr->Header);
		if (e) return e;
		ptr->size += ptr->Header->size;
	}
	if (ptr->Aperture) {
		e = gf_isom_box_size((GF_Box *) ptr->Aperture);
		if (e) return e;
		ptr->size += ptr->Aperture->size;
	}
	if (ptr->udta) {
		e = gf_isom_box_size((GF_Box *) ptr->udta);
		if (e) return e;
		ptr->size += ptr->udta->size;
	}
	if (ptr->References) {
		e = gf_isom_box_size((GF_Box *) ptr->References);
		if (e) return e;
		ptr->size += ptr->References->size;
	}
	if (ptr->editBox) {
		e = gf_isom_box_size((GF_Box *) ptr->editBox);
		if (e) return e;
		ptr->size += ptr->editBox->size;
	}
	if (ptr->Media) {
		e = gf_isom_box_size((GF_Box *) ptr->Media);
		if (e) return e;
		ptr->size += ptr->Media->size;
	}
	if (ptr->meta) {
		e = gf_isom_box_size((GF_Box *) ptr->meta);
		if (e) return e;
		ptr->size += ptr->meta->size;
	}
	if (ptr->groups) {
		e = gf_isom_box_size((GF_Box *) ptr->groups);
		if (e) return e;
		ptr->size += ptr->groups->size;
	}
	return GF_OK;