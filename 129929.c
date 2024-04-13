static gpointer
resolve_object (MonoImage *image, MonoObject *obj, MonoClass **handle_class, MonoGenericContext *context)
{
	gpointer result = NULL;

	if (strcmp (obj->vtable->klass->name, "String") == 0) {
		result = mono_string_intern ((MonoString*)obj);
		*handle_class = mono_defaults.string_class;
		g_assert (result);
	} else if (strcmp (obj->vtable->klass->name, "MonoType") == 0) {
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType*)obj);
		MonoClass *mc = mono_class_from_mono_type (type);
		if (!mono_class_init (mc))
			mono_raise_exception (mono_class_get_exception_for_failure (mc));

		if (context) {
			MonoType *inflated = mono_class_inflate_generic_type (type, context);
			result = mono_class_from_mono_type (inflated);
			mono_metadata_free_type (inflated);
		} else {
			result = mono_class_from_mono_type (type);
		}
		*handle_class = mono_defaults.typehandle_class;
		g_assert (result);
	} else if (strcmp (obj->vtable->klass->name, "MonoMethod") == 0 ||
		   strcmp (obj->vtable->klass->name, "MonoCMethod") == 0 ||
		   strcmp (obj->vtable->klass->name, "MonoGenericCMethod") == 0 ||
		   strcmp (obj->vtable->klass->name, "MonoGenericMethod") == 0) {
		result = ((MonoReflectionMethod*)obj)->method;
		if (context)
			result = mono_class_inflate_generic_method (result, context);
		*handle_class = mono_defaults.methodhandle_class;
		g_assert (result);
	} else if (strcmp (obj->vtable->klass->name, "MethodBuilder") == 0) {
		MonoReflectionMethodBuilder *mb = (MonoReflectionMethodBuilder*)obj;
		result = mb->mhandle;
		if (!result) {
			/* Type is not yet created */
			MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder*)mb->type;

			mono_domain_try_type_resolve (mono_domain_get (), NULL, (MonoObject*)tb);

			/*
			 * Hopefully this has been filled in by calling CreateType() on the
			 * TypeBuilder.
			 */
			/*
			 * TODO: This won't work if the application finishes another 
			 * TypeBuilder instance instead of this one.
			 */
			result = mb->mhandle;
		}
		if (context)
			result = mono_class_inflate_generic_method (result, context);
		*handle_class = mono_defaults.methodhandle_class;
	} else if (strcmp (obj->vtable->klass->name, "ConstructorBuilder") == 0) {
		MonoReflectionCtorBuilder *cb = (MonoReflectionCtorBuilder*)obj;

		result = cb->mhandle;
		if (!result) {
			MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder*)cb->type;

			mono_domain_try_type_resolve (mono_domain_get (), NULL, (MonoObject*)tb);
			result = cb->mhandle;
		}
		if (context)
			result = mono_class_inflate_generic_method (result, context);
		*handle_class = mono_defaults.methodhandle_class;
	} else if (strcmp (obj->vtable->klass->name, "MonoField") == 0) {
		MonoClassField *field = ((MonoReflectionField*)obj)->field;

		ensure_complete_type (field->parent);
		if (context) {
			MonoType *inflated = mono_class_inflate_generic_type (&field->parent->byval_arg, context);
			MonoClass *class = mono_class_from_mono_type (inflated);
			MonoClassField *inflated_field;
			gpointer iter = NULL;
			mono_metadata_free_type (inflated);
			while ((inflated_field = mono_class_get_fields (class, &iter))) {
				if (!strcmp (field->name, inflated_field->name))
					break;
			}
			g_assert (inflated_field && !strcmp (field->name, inflated_field->name));
			result = inflated_field;
		} else {
			result = field;
		}
		*handle_class = mono_defaults.fieldhandle_class;
		g_assert (result);
	} else if (strcmp (obj->vtable->klass->name, "FieldBuilder") == 0) {
		MonoReflectionFieldBuilder *fb = (MonoReflectionFieldBuilder*)obj;
		result = fb->handle;

		if (!result) {
			MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder*)fb->typeb;

			mono_domain_try_type_resolve (mono_domain_get (), NULL, (MonoObject*)tb);
			result = fb->handle;
		}

		if (fb->handle && fb->handle->parent->generic_container) {
			MonoClass *klass = fb->handle->parent;
			MonoType *type = mono_class_inflate_generic_type (&klass->byval_arg, context);
			MonoClass *inflated = mono_class_from_mono_type (type);

			result = mono_class_get_field_from_name (inflated, mono_field_get_name (fb->handle));
			g_assert (result);
			mono_metadata_free_type (type);
		}
		*handle_class = mono_defaults.fieldhandle_class;
	} else if (strcmp (obj->vtable->klass->name, "TypeBuilder") == 0) {
		MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder*)obj;
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType*)tb); 
		MonoClass *klass;

		klass = type->data.klass;
		if (klass->wastypebuilder) {
			/* Already created */
			result = klass;
		}
		else {
			mono_domain_try_type_resolve (mono_domain_get (), NULL, (MonoObject*)tb);
			result = type->data.klass;
			g_assert (result);
		}
		*handle_class = mono_defaults.typehandle_class;
	} else if (strcmp (obj->vtable->klass->name, "SignatureHelper") == 0) {
		MonoReflectionSigHelper *helper = (MonoReflectionSigHelper*)obj;
		MonoMethodSignature *sig;
		int nargs, i;

		if (helper->arguments)
			nargs = mono_array_length (helper->arguments);
		else
			nargs = 0;

		sig = mono_metadata_signature_alloc (image, nargs);
		sig->explicit_this = helper->call_conv & 64 ? 1 : 0;
		sig->hasthis = helper->call_conv & 32 ? 1 : 0;

		if (helper->unmanaged_call_conv) { /* unmanaged */
			sig->call_convention = helper->unmanaged_call_conv - 1;
			sig->pinvoke = TRUE;
		} else if (helper->call_conv & 0x02) {
			sig->call_convention = MONO_CALL_VARARG;
		} else {
			sig->call_convention = MONO_CALL_DEFAULT;
		}

		sig->param_count = nargs;
		/* TODO: Copy type ? */
		sig->ret = helper->return_type->type;
		for (i = 0; i < nargs; ++i)
			sig->params [i] = mono_type_array_get_and_resolve (helper->arguments, i);

		result = sig;
		*handle_class = NULL;
	} else if (strcmp (obj->vtable->klass->name, "DynamicMethod") == 0) {
		MonoReflectionDynamicMethod *method = (MonoReflectionDynamicMethod*)obj;
		/* Already created by the managed code */
		g_assert (method->mhandle);
		result = method->mhandle;
		*handle_class = mono_defaults.methodhandle_class;
	} else if (strcmp (obj->vtable->klass->name, "GenericTypeParameterBuilder") == 0) {
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType*)obj);
		type = mono_class_inflate_generic_type (type, context);
		result = mono_class_from_mono_type (type);
		*handle_class = mono_defaults.typehandle_class;
		g_assert (result);
		mono_metadata_free_type (type);
	} else if (strcmp (obj->vtable->klass->name, "MonoGenericClass") == 0) {
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType*)obj);
		type = mono_class_inflate_generic_type (type, context);
		result = mono_class_from_mono_type (type);
		*handle_class = mono_defaults.typehandle_class;
		g_assert (result);
		mono_metadata_free_type (type);
	} else if (strcmp (obj->vtable->klass->name, "FieldOnTypeBuilderInst") == 0) {
		MonoReflectionFieldOnTypeBuilderInst *f = (MonoReflectionFieldOnTypeBuilderInst*)obj;
		MonoClass *inflated;
		MonoType *type;
		MonoClassField *field;

		if (is_sre_field_builder (mono_object_class (f->fb)))
			field = ((MonoReflectionFieldBuilder*)f->fb)->handle;
		else if (is_sr_mono_field (mono_object_class (f->fb)))
			field = ((MonoReflectionField*)f->fb)->field;
		else
			g_error ("resolve_object:: can't handle a FTBI with base_method of type %s", mono_type_get_full_name (mono_object_class (f->fb)));

		type = mono_class_inflate_generic_type (mono_reflection_type_get_handle ((MonoReflectionType*)f->inst), context);
		inflated = mono_class_from_mono_type (type);

		result = field = mono_class_get_field_from_name (inflated, mono_field_get_name (field));
		ensure_complete_type (field->parent);
		g_assert (result);
		mono_metadata_free_type (type);
		*handle_class = mono_defaults.fieldhandle_class;
	} else if (strcmp (obj->vtable->klass->name, "ConstructorOnTypeBuilderInst") == 0) {
		MonoReflectionCtorOnTypeBuilderInst *c = (MonoReflectionCtorOnTypeBuilderInst*)obj;
		MonoType *type = mono_class_inflate_generic_type (mono_reflection_type_get_handle ((MonoReflectionType*)c->inst), context);
		MonoClass *inflated_klass = mono_class_from_mono_type (type);
		MonoMethod *method;

		if (is_sre_ctor_builder (mono_object_class (c->cb)))
			method = ((MonoReflectionCtorBuilder *)c->cb)->mhandle;
		else if (is_sr_mono_cmethod (mono_object_class (c->cb)))
			method = ((MonoReflectionMethod *)c->cb)->method;
		else
			g_error ("resolve_object:: can't handle a CTBI with base_method of type %s", mono_type_get_full_name (mono_object_class (c->cb)));

		result = inflate_mono_method (inflated_klass, method, (MonoObject*)c->cb);
		*handle_class = mono_defaults.methodhandle_class;
		mono_metadata_free_type (type);
	} else if (strcmp (obj->vtable->klass->name, "MethodOnTypeBuilderInst") == 0) {
		MonoReflectionMethodOnTypeBuilderInst *m = (MonoReflectionMethodOnTypeBuilderInst*)obj;
		if (m->method_args) {
			result = mono_reflection_method_on_tb_inst_get_handle (m);
			if (context)
				result = mono_class_inflate_generic_method (result, context);
		} else {
			MonoType *type = mono_class_inflate_generic_type (mono_reflection_type_get_handle ((MonoReflectionType*)m->inst), context);
			MonoClass *inflated_klass = mono_class_from_mono_type (type);
			MonoMethod *method;

			if (is_sre_method_builder (mono_object_class (m->mb)))
				method = ((MonoReflectionMethodBuilder *)m->mb)->mhandle;
 			else if (is_sr_mono_method (mono_object_class (m->mb)))
				method = ((MonoReflectionMethod *)m->mb)->method;
			else
				g_error ("resolve_object:: can't handle a MTBI with base_method of type %s", mono_type_get_full_name (mono_object_class (m->mb)));

			result = inflate_mono_method (inflated_klass, method, (MonoObject*)m->mb);
			mono_metadata_free_type (type);
		}
		*handle_class = mono_defaults.methodhandle_class;
	} else if (strcmp (obj->vtable->klass->name, "MonoArrayMethod") == 0) {
		MonoReflectionArrayMethod *m = (MonoReflectionArrayMethod*)obj;
		MonoType *mtype;
		MonoClass *klass;
		MonoMethod *method;
		gpointer iter;
		char *name;

		mtype = mono_reflection_type_get_handle (m->parent);
		klass = mono_class_from_mono_type (mtype);

		/* Find the method */

		name = mono_string_to_utf8 (m->name);
		iter = NULL;
		while ((method = mono_class_get_methods (klass, &iter))) {
			if (!strcmp (method->name, name))
				break;
		}
		g_free (name);

		// FIXME:
		g_assert (method);
		// FIXME: Check parameters/return value etc. match

		result = method;
		*handle_class = mono_defaults.methodhandle_class;
	} else if (is_sre_array (mono_object_get_class(obj)) ||
				is_sre_byref (mono_object_get_class(obj)) ||
				is_sre_pointer (mono_object_get_class(obj))) {
		MonoReflectionType *ref_type = (MonoReflectionType *)obj;
		MonoType *type = mono_reflection_type_get_handle (ref_type);
		result = mono_class_from_mono_type (type);
		*handle_class = mono_defaults.typehandle_class;
	} else {
		g_print ("%s\n", obj->vtable->klass->name);
		g_assert_not_reached ();
	}
	return result;