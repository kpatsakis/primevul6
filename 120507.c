GF_Box *stco_New()
{
	ISOM_DECL_BOX_ALLOC(GF_ChunkOffsetBox, GF_ISOM_BOX_TYPE_STCO);
	return (GF_Box *)tmp;
}