GF_Box *edts_New()
{
	ISOM_DECL_BOX_ALLOC(GF_EditBox, GF_ISOM_BOX_TYPE_EDTS);
	return (GF_Box *) tmp;
}