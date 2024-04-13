dump_stack_value (ILStackDesc *value)
{
	printf ("[(%x)(%x)", value->type->type, value->stype);

	if (stack_slot_is_this_pointer (value))
		printf ("[this] ");

	if (stack_slot_is_boxed_value (value))
		printf ("[boxed] ");

	if (stack_slot_is_null_literal (value))
		printf ("[null] ");

	if (stack_slot_is_managed_mutability_pointer (value))
		printf ("Controled Mutability MP: ");

	if (stack_slot_is_managed_pointer (value))
		printf ("Managed Pointer to: ");

	switch (stack_slot_get_underlying_type (value)) {
		case TYPE_INV:
			printf ("invalid type]"); 
			return;
		case TYPE_I4:
			printf ("int32]"); 
			return;
		case TYPE_I8:
			printf ("int64]"); 
			return;
		case TYPE_NATIVE_INT:
			printf ("native int]"); 
			return;
		case TYPE_R8:
			printf ("float64]"); 
			return;
		case TYPE_PTR:
			printf ("unmanaged pointer]"); 
			return;
		case TYPE_COMPLEX:
			switch (value->type->type) {
			case MONO_TYPE_CLASS:
			case MONO_TYPE_VALUETYPE:
				printf ("complex] (%s)", value->type->data.klass->name);
				return;
			case MONO_TYPE_STRING:
				printf ("complex] (string)");
				return;
			case MONO_TYPE_OBJECT:
				printf ("complex] (object)");
				return;
			case MONO_TYPE_SZARRAY:
				printf ("complex] (%s [])", value->type->data.klass->name);
				return;
			case MONO_TYPE_ARRAY:
				printf ("complex] (%s [%d %d %d])",
					value->type->data.array->eklass->name,
					value->type->data.array->rank,
					value->type->data.array->numsizes,
					value->type->data.array->numlobounds);
				return;
			case MONO_TYPE_GENERICINST:
				printf ("complex] (inst of %s )", value->type->data.generic_class->container_class->name);
				return;
			case MONO_TYPE_VAR:
				printf ("complex] (type generic param !%d - %s) ", value->type->data.generic_param->num, mono_generic_param_info (value->type->data.generic_param)->name);
				return;
			case MONO_TYPE_MVAR:
				printf ("complex] (method generic param !!%d - %s) ", value->type->data.generic_param->num, mono_generic_param_info (value->type->data.generic_param)->name);
				return;
			default: {
				//should be a boxed value 
				char * name = mono_type_full_name (value->type);
				printf ("complex] %s", name);
				g_free (name);
				return;
				}
			}
		default:
			printf ("unknown stack %x type]\n", value->stype);
			g_assert_not_reached ();
	}
}