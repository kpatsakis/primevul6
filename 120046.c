GF_Err moov_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MovieBox *ptr = (GF_MovieBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	if (ptr->mvhd) {
		e = gf_isom_box_write((GF_Box *) ptr->mvhd, bs);
		if (e) return e;
	}
	if (ptr->iods) {
		e = gf_isom_box_write((GF_Box *) ptr->iods, bs);
		if (e) return e;
	}
	if (ptr->meta) {
		e = gf_isom_box_write((GF_Box *) ptr->meta, bs);
		if (e) return e;
	}
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (ptr->mvex && !ptr->mvex_after_traks) {
		e = gf_isom_box_write((GF_Box *) ptr->mvex, bs);
		if (e) return e;
	}
#endif

	e = gf_isom_box_array_write(s, ptr->trackList, bs);
	if (e) return e;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (ptr->mvex && ptr->mvex_after_traks) {
		e = gf_isom_box_write((GF_Box *) ptr->mvex, bs);
		if (e) return e;
	}
#endif

	if (ptr->udta) {
		e = gf_isom_box_write((GF_Box *) ptr->udta, bs);
		if (e) return e;
	}
	return GF_OK;
}