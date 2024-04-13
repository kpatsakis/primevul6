GF_Box *vvnc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_VVCNaluConfigurationBox, GF_ISOM_BOX_TYPE_VVNC);
	return (GF_Box *)tmp;
}