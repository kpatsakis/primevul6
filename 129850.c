type_get_fully_qualified_name (MonoType *type)
{
	return mono_type_get_name_full (type, MONO_TYPE_NAME_FORMAT_ASSEMBLY_QUALIFIED);
}