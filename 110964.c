const char *gf_m4a_object_type_name(u32 objectType)
{
	u32 i, count = GF_ARRAY_LENGTH(M4AObjectTypes);
	for (i=0; i<count; i++) {
		if (objectType==M4AObjectTypes[i].type)
			return M4AObjectTypes[i].name;
	}
	return "MPEG-4 Audio Unknown";
}