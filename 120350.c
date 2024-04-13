
static void	sgpd_del_entry(u32 grouping_type, void *entry)
{
	switch (grouping_type) {
	case GF_ISOM_SAMPLE_GROUP_SYNC:
	case GF_ISOM_SAMPLE_GROUP_ROLL:
	case GF_ISOM_SAMPLE_GROUP_PROL:
	case GF_ISOM_SAMPLE_GROUP_RAP:
	case GF_ISOM_SAMPLE_GROUP_SEIG:
	case GF_ISOM_SAMPLE_GROUP_TELE:
	case GF_ISOM_SAMPLE_GROUP_SAP:
		gf_free(entry);
		return;
	case GF_ISOM_SAMPLE_GROUP_OINF:
		gf_isom_oinf_del_entry(entry);
		return;
	case GF_ISOM_SAMPLE_GROUP_LINF:
		gf_isom_linf_del_entry(entry);
		return;
	default:
	{
		GF_DefaultSampleGroupDescriptionEntry *ptr = (GF_DefaultSampleGroupDescriptionEntry *)entry;
		if (ptr->data) gf_free(ptr->data);
		gf_free(ptr);
	}
	}