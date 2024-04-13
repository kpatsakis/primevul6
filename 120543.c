GF_Err moov_Size(GF_Box *s)
{
	GF_Err e;
	GF_MovieBox *ptr = (GF_MovieBox *)s;

	if (ptr->mvhd) {
		e = gf_isom_box_size((GF_Box *) ptr->mvhd);
		if (e) return e;
		ptr->size += ptr->mvhd->size;
	}
	if (ptr->iods) {
		e = gf_isom_box_size((GF_Box *) ptr->iods);
		if (e) return e;
		ptr->size += ptr->iods->size;
	}
	if (ptr->udta) {
		e = gf_isom_box_size((GF_Box *) ptr->udta);
		if (e) return e;
		ptr->size += ptr->udta->size;
	}
	if (ptr->meta) {
		e = gf_isom_box_size((GF_Box *) ptr->meta);
		if (e) return e;
		ptr->size += ptr->meta->size;
	}
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (ptr->mvex) {
		e = gf_isom_box_size((GF_Box *) ptr->mvex);
		if (e) return e;
		ptr->size += ptr->mvex->size;
	}
#endif

	return gf_isom_box_array_size(s, ptr->trackList);
}