
GF_Err pdin_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_ProgressiveDownloadBox *ptr = (GF_ProgressiveDownloadBox*)s;

	ptr->count = (u32) (ptr->size) / 8;
	ptr->rates = (u32*)gf_malloc(sizeof(u32)*ptr->count);
	ptr->times = (u32*)gf_malloc(sizeof(u32)*ptr->count);
	for (i=0; i<ptr->count; i++) {
		ptr->rates[i] = gf_bs_read_u32(bs);
		ptr->times[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;