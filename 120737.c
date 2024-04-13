GF_Box *ctts_New()
{
	ISOM_DECL_BOX_ALLOC(GF_CompositionOffsetBox, GF_ISOM_BOX_TYPE_CTTS);
	return (GF_Box *) tmp;
}