Bool gf_isom_is_encrypted_entry(u32 entryType)
{
	switch (entryType) {
	case GF_ISOM_BOX_TYPE_ENCA:
	case GF_ISOM_BOX_TYPE_ENCV:
	case GF_ISOM_BOX_TYPE_ENCS:
		return GF_TRUE;
	default:
		return GF_FALSE;
	}
}