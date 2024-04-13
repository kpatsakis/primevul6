
GF_Err paen_AddBox(GF_Box *s, GF_Box *a)
{
	FDPartitionEntryBox *ptr = (FDPartitionEntryBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_FPAR:
		if (ptr->blocks_and_symbols) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->blocks_and_symbols = (FilePartitionBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_FECR:
		if (ptr->FEC_symbol_locations) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->FEC_symbol_locations = (FECReservoirBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_FIRE:
		if (ptr->File_symbol_locations) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->File_symbol_locations = (FileReservoirBox *)a;
		return GF_OK;
	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;