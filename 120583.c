GF_Err mp4s_AddBox(GF_Box *s, GF_Box *a)
{
	GF_MPEGSampleEntryBox *ptr = (GF_MPEGSampleEntryBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_ESDS:
		if (ptr->esd) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->esd = (GF_ESDBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_SINF:
		gf_list_add(ptr->protections, a);
		break;
	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;
}