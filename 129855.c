MonoType*
mono_reflection_type_get_handle (MonoReflectionType* ref)
{
	MonoClass *class;
	if (!ref)
		return NULL;
	if (ref->type)
		return ref->type;

	if (is_usertype (ref)) {
		ref = mono_reflection_type_get_underlying_system_type (ref);
		if (ref == NULL || is_usertype (ref))
			return NULL;
		if (ref->type)
			return ref->type;
	}

	class = mono_object_class (ref);

	if (is_sre_array (class)) {
		MonoType *res;
		MonoReflectionArrayType *sre_array = (MonoReflectionArrayType*)ref;
		MonoType *base = mono_reflection_type_get_handle (sre_array->element_type);
		g_assert (base);
		if (sre_array->rank == 0) //single dimentional array
			res = &mono_array_class_get (mono_class_from_mono_type (base), 1)->byval_arg;
		else
			res = &mono_bounded_array_class_get (mono_class_from_mono_type (base), sre_array->rank, TRUE)->byval_arg;
		sre_array->type.type = res;
		return res;
	} else if (is_sre_byref (class)) {
		MonoType *res;
		MonoReflectionDerivedType *sre_byref = (MonoReflectionDerivedType*)ref;
		MonoType *base = mono_reflection_type_get_handle (sre_byref->element_type);
		g_assert (base);
		res = &mono_class_from_mono_type (base)->this_arg;
		sre_byref->type.type = res;
		return res;
	} else if (is_sre_pointer (class)) {
		MonoType *res;
		MonoReflectionDerivedType *sre_pointer = (MonoReflectionDerivedType*)ref;
		MonoType *base = mono_reflection_type_get_handle (sre_pointer->element_type);
		g_assert (base);
		res = &mono_ptr_class_get (base)->byval_arg;
		sre_pointer->type.type = res;
		return res;
	} else if (is_sre_generic_instance (class)) {
		MonoType *res, **types;
		MonoReflectionGenericClass *gclass = (MonoReflectionGenericClass*)ref;
		int i, count;

		count = mono_array_length (gclass->type_arguments);
		types = g_new0 (MonoType*, count);
		for (i = 0; i < count; ++i) {
			MonoReflectionType *t = mono_array_get (gclass->type_arguments, gpointer, i);
			types [i] = mono_reflection_type_get_handle (t);
			if (!types[i]) {
				g_free (types);
				return NULL;
			}
		}

		res = mono_reflection_bind_generic_parameters (gclass->generic_type, count, types);
		g_free (types);
		g_assert (res);
		gclass->type.type = res;
		return res;
	}

	g_error ("Cannot handle corlib user type %s", mono_type_full_name (&mono_object_class(ref)->byval_arg));
	return NULL;