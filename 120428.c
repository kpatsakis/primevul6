GF_Box *padb_New()
{
	ISOM_DECL_BOX_ALLOC(GF_PaddingBitsBox, GF_ISOM_BOX_TYPE_PADB);
	return (GF_Box *)tmp;
}