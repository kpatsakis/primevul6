
GF_Err lsr1_AddBox(GF_Box *s, GF_Box *a)
{
	GF_LASeRSampleEntryBox *ptr = (GF_LASeRSampleEntryBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_LSRC:
		if (ptr->lsr_config) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->lsr_config = (GF_LASERConfigurationBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_M4DS:
		if (ptr->descr) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->descr = (GF_MPEG4ExtensionDescriptorsBox *)a;
		break;
	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;