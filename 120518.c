GF_Box *url_New()
{
	ISOM_DECL_BOX_ALLOC(GF_DataEntryURLBox, GF_ISOM_BOX_TYPE_URL);
	return (GF_Box *)tmp;
}