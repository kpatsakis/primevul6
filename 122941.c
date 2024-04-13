GF_Box *vvcc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_VVCConfigurationBox, GF_ISOM_BOX_TYPE_VVCC);
	return (GF_Box *)tmp;
}