get_stack_type (MonoType *type)
{
	int mask = 0;
	int type_kind = type->type;
	if (type->byref)
		mask = POINTER_MASK;
	/*TODO handle CMMP_MASK */

handle_enum:
	switch (type_kind) {
	case MONO_TYPE_I1:
	case MONO_TYPE_U1:
	case MONO_TYPE_BOOLEAN:
	case MONO_TYPE_I2:
	case MONO_TYPE_U2:
	case MONO_TYPE_CHAR:
	case MONO_TYPE_I4:
	case MONO_TYPE_U4:
		return TYPE_I4 | mask;

	case MONO_TYPE_I:
	case MONO_TYPE_U:
		return TYPE_NATIVE_INT | mask;

	/* FIXME: the spec says that you cannot have a pointer to method pointer, do we need to check this here? */ 
	case MONO_TYPE_FNPTR:
	case MONO_TYPE_PTR:
	case MONO_TYPE_TYPEDBYREF:
		return TYPE_PTR | mask;

	case MONO_TYPE_VAR:
	case MONO_TYPE_MVAR:

	case MONO_TYPE_CLASS:
	case MONO_TYPE_STRING:
	case MONO_TYPE_OBJECT:
	case MONO_TYPE_SZARRAY:
	case MONO_TYPE_ARRAY:
		return TYPE_COMPLEX | mask;

	case MONO_TYPE_I8:
	case MONO_TYPE_U8:
		return TYPE_I8 | mask;

	case MONO_TYPE_R4:
	case MONO_TYPE_R8:
		return TYPE_R8 | mask;

	case MONO_TYPE_GENERICINST:
	case MONO_TYPE_VALUETYPE:
		if (mono_type_is_enum_type (type)) {
			type = mono_type_get_underlying_type_any (type);
			if (!type)
				return FALSE;
			type_kind = type->type;
			goto handle_enum;
		} else {
			return TYPE_COMPLEX | mask;
		}

	default:
		return TYPE_INV;
	}
}