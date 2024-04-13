const char *gf_av1_get_obu_name(ObuType obu_type)
{
	switch (obu_type) {
	case OBU_SEQUENCE_HEADER: return "seq_header";
	case OBU_TEMPORAL_DELIMITER: return "delimiter";
	case OBU_FRAME_HEADER: return "frame_header";
	case OBU_TILE_GROUP: return "tile_group";
	case OBU_METADATA: return "metadata";
	case OBU_FRAME: return "frame";
	case OBU_REDUNDANT_FRAME_HEADER: return "redundant_frame_header";
	case OBU_TILE_LIST: return "tile_list";
	case OBU_PADDING: return "padding";
	case OBU_RESERVED_0:
	case OBU_RESERVED_9:
	case OBU_RESERVED_10:
	case OBU_RESERVED_11:
	case OBU_RESERVED_12:
	case OBU_RESERVED_13:
	case OBU_RESERVED_14:
		return "reserved";
	default: return "unknown";
	}
}