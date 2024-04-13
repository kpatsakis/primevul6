GF_Err mp4s_Size(GF_Box *s)
{
	GF_Err e;
	GF_MPEGSampleEntryBox *ptr = (GF_MPEGSampleEntryBox *)s;

	ptr->size += 8;
	e = gf_isom_box_size((GF_Box *)ptr->esd);
	if (e) return e;
	ptr->size += ptr->esd->size;
	return gf_isom_box_array_size(s, ptr->protections);
}