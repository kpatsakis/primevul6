default_class_from_mono_type (MonoType *type)
{
	switch (type->type) {
	case MONO_TYPE_OBJECT:
		return mono_defaults.object_class;
	case MONO_TYPE_VOID:
		return mono_defaults.void_class;
	case MONO_TYPE_BOOLEAN:
		return mono_defaults.boolean_class;
	case MONO_TYPE_CHAR:
		return mono_defaults.char_class;
	case MONO_TYPE_I1:
		return mono_defaults.sbyte_class;
	case MONO_TYPE_U1:
		return mono_defaults.byte_class;
	case MONO_TYPE_I2:
		return mono_defaults.int16_class;
	case MONO_TYPE_U2:
		return mono_defaults.uint16_class;
	case MONO_TYPE_I4:
		return mono_defaults.int32_class;
	case MONO_TYPE_U4:
		return mono_defaults.uint32_class;
	case MONO_TYPE_I:
		return mono_defaults.int_class;
	case MONO_TYPE_U:
		return mono_defaults.uint_class;
	case MONO_TYPE_I8:
		return mono_defaults.int64_class;
	case MONO_TYPE_U8:
		return mono_defaults.uint64_class;
	case MONO_TYPE_R4:
		return mono_defaults.single_class;
	case MONO_TYPE_R8:
		return mono_defaults.double_class;
	case MONO_TYPE_STRING:
		return mono_defaults.string_class;
	default:
		g_warning ("default_class_from_mono_type: implement me 0x%02x\n", type->type);
		g_assert_not_reached ();
	}
	
	return NULL;
}