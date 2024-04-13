GF_Err gf_isom_set_inplace_padding(GF_ISOFile *file, u32 padding)
{
	if (!file) return GF_BAD_PARAM;
	file->padding = padding;
	return GF_OK;
}