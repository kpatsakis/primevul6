
GF_Err paen_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	FDPartitionEntryBox *ptr = (FDPartitionEntryBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	if (ptr->blocks_and_symbols) {
		e = gf_isom_box_write((GF_Box *)ptr->blocks_and_symbols, bs);
		if (e) return e;
	}
	if (ptr->FEC_symbol_locations) {
		e = gf_isom_box_write((GF_Box *)ptr->FEC_symbol_locations, bs);
		if (e) return e;
	}
	if (ptr->File_symbol_locations) {
		e = gf_isom_box_write((GF_Box *)ptr->File_symbol_locations, bs);
		if (e) return e;
	}
	return GF_OK;