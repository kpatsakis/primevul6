
GF_Err fecr_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	FECReservoirBox *ptr = (FECReservoirBox *)s;

	ISOM_DECREASE_SIZE(ptr, (ptr->version ? 4 : 2) );
	ptr->nb_entries = gf_bs_read_int(bs, ptr->version ? 32 : 16);

	ISOM_DECREASE_SIZE(ptr, ptr->nb_entries * (ptr->version ? 8 : 6) );
	GF_SAFE_ALLOC_N(ptr->entries, ptr->nb_entries, FECReservoirEntry);
	for (i=0; i<ptr->nb_entries; i++) {
		ptr->entries[i].item_id = gf_bs_read_int(bs, ptr->version ? 32 : 16);
		ptr->entries[i].symbol_count = gf_bs_read_u32(bs);
	}
	return GF_OK;