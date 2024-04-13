Bool av1_is_obu_header(ObuType obu_type) {
	switch (obu_type) {
	case OBU_SEQUENCE_HEADER:
	case OBU_METADATA:
		// TODO add check based on the metadata type
		return GF_TRUE;
	default:
		return GF_FALSE;
	}
}