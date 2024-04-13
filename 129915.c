is_valid_bool_arg (ILStackDesc *arg)
{
	if (stack_slot_is_managed_pointer (arg) || stack_slot_is_boxed_value (arg) || stack_slot_is_null_literal (arg))
		return TRUE;


	switch (stack_slot_get_underlying_type (arg)) {
	case TYPE_I4:
	case TYPE_I8:
	case TYPE_NATIVE_INT:
	case TYPE_PTR:
		return TRUE;
	case TYPE_COMPLEX:
		g_assert (arg->type);
		switch (arg->type->type) {
		case MONO_TYPE_CLASS:
		case MONO_TYPE_STRING:
		case MONO_TYPE_OBJECT:
		case MONO_TYPE_SZARRAY:
		case MONO_TYPE_ARRAY:
		case MONO_TYPE_FNPTR:
		case MONO_TYPE_PTR:
			return TRUE;
		case MONO_TYPE_GENERICINST:
			/*We need to check if the container class
			 * of the generic type is a valuetype, iow:
			 * is it a "class Foo<T>" or a "struct Foo<T>"?
			 */
			return !arg->type->data.generic_class->container_class->valuetype;
		}
	default:
		return FALSE;
	}
}