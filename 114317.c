Bool IsMP4Description(u32 entryType)
{
	switch (entryType) {
	case GF_ISOM_BOX_TYPE_MP4S:
	case GF_ISOM_BOX_TYPE_LSR1:
	case GF_ISOM_BOX_TYPE_MP4A:
	case GF_ISOM_BOX_TYPE_MP4V:
	case GF_ISOM_BOX_TYPE_ENCA:
	case GF_ISOM_BOX_TYPE_ENCV:
	case GF_ISOM_BOX_TYPE_RESV:
	case GF_ISOM_BOX_TYPE_ENCS:
		return GF_TRUE;
	default:
		return GF_FALSE;
	}
}