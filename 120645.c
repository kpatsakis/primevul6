GF_Err ftyp_Read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_FileTypeBox *ptr = (GF_FileTypeBox *)s;

	if (ptr->size < 8) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Found ftyp with size < 8, likely broken!\n"));
		return GF_BAD_PARAM;
	}
	ptr->majorBrand = gf_bs_read_u32(bs);
	ptr->minorVersion = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 8);

	ptr->altCount = ( (u32) (ptr->size)) / 4;
	if (!ptr->altCount) return GF_OK;
	if (ptr->altCount * 4 != (u32) (ptr->size)) return GF_ISOM_INVALID_FILE;

	ptr->altBrand = (u32*)gf_malloc(sizeof(u32)*ptr->altCount);
	for (i = 0; i<ptr->altCount; i++) {
		ptr->altBrand[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;
}