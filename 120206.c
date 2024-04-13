
GF_Err trak_AddBox(GF_Box *s, GF_Box *a)
{
	GF_TrackBox *ptr = (GF_TrackBox *)s;
	if (!a) return GF_OK;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_TKHD:
		if (ptr->Header) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->Header = (GF_TrackHeaderBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_EDTS:
		if (ptr->editBox) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->editBox = (GF_EditBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_UDTA:
		if (ptr->udta) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->udta = (GF_UserDataBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_META:
		if (ptr->meta) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->meta = (GF_MetaBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_TREF:
		if (ptr->References) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->References = (GF_TrackReferenceBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_MDIA:
		if (ptr->Media) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->Media = (GF_MediaBox *)a;
		((GF_MediaBox *)a)->mediaTrack = ptr;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_TRGR:
		if (ptr->groups) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->groups = (GF_TrackGroupBox *)a;
		return GF_OK;
	case GF_QT_BOX_TYPE_TAPT:
		if (ptr->Aperture) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->Aperture = (GF_Box *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_SENC:
		ptr->sample_encryption = (GF_SampleEncryptionBox*)a;
		return gf_isom_box_add_default((GF_Box *)ptr, a);
	case GF_ISOM_BOX_TYPE_UUID:
		if (((GF_UnknownUUIDBox *)a)->internal_4cc == GF_ISOM_BOX_UUID_PSEC) {
			ptr->sample_encryption = (GF_SampleEncryptionBox*) a;
			return gf_isom_box_add_default((GF_Box *)ptr, a);
		}

	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;