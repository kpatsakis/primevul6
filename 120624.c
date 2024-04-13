GF_Err stsh_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 count, i;
	GF_StshEntry *ent;
	GF_ShadowSyncBox *ptr = (GF_ShadowSyncBox *)s;

	count = gf_bs_read_u32(bs);

	for (i = 0; i < count; i++) {
		ent = (GF_StshEntry *) gf_malloc(sizeof(GF_StshEntry));
		if (!ent) return GF_OUT_OF_MEM;
		ent->shadowedSampleNumber = gf_bs_read_u32(bs);
		ent->syncSampleNumber = gf_bs_read_u32(bs);
		e = gf_list_add(ptr->entries, ent);
		if (e) return e;
	}
	return GF_OK;
}