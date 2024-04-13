#ifndef GPAC_DISABLE_ISOM_WRITE
static u32 sgpd_size_entry(u32 grouping_type, void *entry)
{
	switch (grouping_type) {
	case GF_ISOM_SAMPLE_GROUP_ROLL:
	case GF_ISOM_SAMPLE_GROUP_PROL:
		return 2;
	case GF_ISOM_SAMPLE_GROUP_TELE:
	case GF_ISOM_SAMPLE_GROUP_RAP:
	case GF_ISOM_SAMPLE_GROUP_SAP:
	case GF_ISOM_SAMPLE_GROUP_SYNC:
		return 1;
	case GF_ISOM_SAMPLE_GROUP_TSCL:
		return 20;
	case GF_ISOM_SAMPLE_GROUP_LBLI:
		return 2;
	case GF_ISOM_SAMPLE_GROUP_TSAS:
	case GF_ISOM_SAMPLE_GROUP_STSA:
		return 0;
	case GF_ISOM_SAMPLE_GROUP_SEIG:
		return ((((GF_CENCSampleEncryptionGroupEntry *)entry)->IsProtected == 1) && !((GF_CENCSampleEncryptionGroupEntry *)entry)->Per_Sample_IV_size) ? 21 + ((GF_CENCSampleEncryptionGroupEntry *)entry)->constant_IV_size : 20;
	case GF_ISOM_SAMPLE_GROUP_OINF:
		return gf_isom_oinf_size_entry(entry);
	case GF_ISOM_SAMPLE_GROUP_LINF:
		return gf_isom_linf_size_entry(entry);
	default:
		return ((GF_DefaultSampleGroupDescriptionEntry *)entry)->length;
	}