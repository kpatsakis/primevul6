GF_Err stsh_Size(GF_Box *s)
{
	GF_ShadowSyncBox *ptr = (GF_ShadowSyncBox *)s;
	ptr->size += 4 + (8 * gf_list_count(ptr->entries));
	return GF_OK;
}