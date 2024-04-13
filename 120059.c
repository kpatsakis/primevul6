GF_Box *free_New()
{
	ISOM_DECL_BOX_ALLOC(GF_FreeSpaceBox, GF_ISOM_BOX_TYPE_FREE);
	return (GF_Box *)tmp;
}