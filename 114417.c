Bool gf_isom_has_segment(GF_ISOFile *file, u32 *brand, u32 *version)
{
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	u32 i;
	GF_Box *a;
	i = 0;
	while (NULL != (a = (GF_Box*)gf_list_enum(file->TopBoxes, &i))) {
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
		if (a->type == GF_ISOM_BOX_TYPE_STYP) {
			GF_FileTypeBox *styp = (GF_FileTypeBox *)a;
			*brand = styp->majorBrand;
			*version = styp->minorVersion;
			return GF_TRUE;
		}
#endif
	}
#endif
	return GF_FALSE;
}