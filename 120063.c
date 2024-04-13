void moov_del(GF_Box *s)
{
	GF_MovieBox *ptr = (GF_MovieBox *)s;
	if (ptr == NULL) return;

	if (ptr->mvhd) gf_isom_box_del((GF_Box *)ptr->mvhd);
	if (ptr->meta) gf_isom_box_del((GF_Box *)ptr->meta);
	if (ptr->iods) gf_isom_box_del((GF_Box *)ptr->iods);
	if (ptr->udta) gf_isom_box_del((GF_Box *)ptr->udta);
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (ptr->mvex) gf_isom_box_del((GF_Box *)ptr->mvex);
#endif

	gf_isom_box_array_del(ptr->trackList);
	gf_free(ptr);
}