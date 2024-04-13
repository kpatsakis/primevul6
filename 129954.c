mymono_metadata_type_equal (MonoType *t1, MonoType *t2)
{
	if ((t1->type != t2->type) ||
	    (t1->byref != t2->byref))
		return FALSE;

	switch (t1->type) {
	case MONO_TYPE_VOID:
	case MONO_TYPE_BOOLEAN:
	case MONO_TYPE_CHAR:
	case MONO_TYPE_I1:
	case MONO_TYPE_U1:
	case MONO_TYPE_I2:
	case MONO_TYPE_U2:
	case MONO_TYPE_I4:
	case MONO_TYPE_U4:
	case MONO_TYPE_I8:
	case MONO_TYPE_U8:
	case MONO_TYPE_R4:
	case MONO_TYPE_R8:
	case MONO_TYPE_STRING:
	case MONO_TYPE_I:
	case MONO_TYPE_U:
	case MONO_TYPE_OBJECT:
	case MONO_TYPE_TYPEDBYREF:
		return TRUE;
	case MONO_TYPE_VALUETYPE:
	case MONO_TYPE_CLASS:
	case MONO_TYPE_SZARRAY:
		return t1->data.klass == t2->data.klass;
	case MONO_TYPE_PTR:
		return mymono_metadata_type_equal (t1->data.type, t2->data.type);
	case MONO_TYPE_ARRAY:
		if (t1->data.array->rank != t2->data.array->rank)
			return FALSE;
		return t1->data.array->eklass == t2->data.array->eklass;
	case MONO_TYPE_GENERICINST: {
		int i;
		MonoGenericInst *i1 = t1->data.generic_class->context.class_inst;
		MonoGenericInst *i2 = t2->data.generic_class->context.class_inst;
		if (i1->type_argc != i2->type_argc)
			return FALSE;
		if (!mono_metadata_type_equal (&t1->data.generic_class->container_class->byval_arg,
					       &t2->data.generic_class->container_class->byval_arg))
			return FALSE;
		/* FIXME: we should probably just compare the instance pointers directly.  */
		for (i = 0; i < i1->type_argc; ++i) {
			if (!mono_metadata_type_equal (i1->type_argv [i], i2->type_argv [i]))
				return FALSE;
		}
		return TRUE;
	}
	case MONO_TYPE_VAR:
	case MONO_TYPE_MVAR:
		return t1->data.generic_param == t2->data.generic_param;
	default:
		g_error ("implement type compare for %0x!", t1->type);
		return FALSE;
	}

	return FALSE;
}