void
mono_reflection_generic_class_initialize (MonoReflectionGenericClass *type, MonoArray *methods, 
					  MonoArray *ctors, MonoArray *fields, MonoArray *properties,
					  MonoArray *events)
{
	MonoGenericClass *gclass;
	MonoDynamicGenericClass *dgclass;
	MonoClass *klass, *gklass;
	MonoType *gtype;
	int i;

	MONO_ARCH_SAVE_REGS;

	gtype = mono_reflection_type_get_handle ((MonoReflectionType*)type);
	klass = mono_class_from_mono_type (gtype);
	g_assert (gtype->type == MONO_TYPE_GENERICINST);
	gclass = gtype->data.generic_class;

	if (!gclass->is_dynamic)
		return;

	dgclass = (MonoDynamicGenericClass *) gclass;

	if (dgclass->initialized)
		return;

	gklass = gclass->container_class;
	mono_class_init (gklass);

	dgclass->count_methods = methods ? mono_array_length (methods) : 0;
	dgclass->count_ctors = ctors ? mono_array_length (ctors) : 0;
	dgclass->count_fields = fields ? mono_array_length (fields) : 0;

	dgclass->methods = g_new0 (MonoMethod *, dgclass->count_methods);
	dgclass->ctors = g_new0 (MonoMethod *, dgclass->count_ctors);
	dgclass->fields = g_new0 (MonoClassField, dgclass->count_fields);
	dgclass->field_objects = g_new0 (MonoObject*, dgclass->count_fields);
	dgclass->field_generic_types = g_new0 (MonoType*, dgclass->count_fields);

	for (i = 0; i < dgclass->count_methods; i++) {
		MonoObject *obj = mono_array_get (methods, gpointer, i);

		dgclass->methods [i] = inflate_method ((MonoReflectionType*)type, obj);
	}

	for (i = 0; i < dgclass->count_ctors; i++) {
		MonoObject *obj = mono_array_get (ctors, gpointer, i);

		dgclass->ctors [i] = inflate_method ((MonoReflectionType*)type, obj);
	}

	for (i = 0; i < dgclass->count_fields; i++) {
		MonoObject *obj = mono_array_get (fields, gpointer, i);
		MonoClassField *field, *inflated_field = NULL;

		if (!strcmp (obj->vtable->klass->name, "FieldBuilder"))
			inflated_field = field = fieldbuilder_to_mono_class_field (klass, (MonoReflectionFieldBuilder *) obj);
		else if (!strcmp (obj->vtable->klass->name, "MonoField"))
			field = ((MonoReflectionField *) obj)->field;
		else {
			field = NULL; /* prevent compiler warning */
			g_assert_not_reached ();
		}

		dgclass->fields [i] = *field;
		dgclass->fields [i].parent = klass;
		dgclass->fields [i].type = mono_class_inflate_generic_type (
			field->type, mono_generic_class_get_context ((MonoGenericClass *) dgclass));
		dgclass->field_generic_types [i] = field->type;
		MOVING_GC_REGISTER (&dgclass->field_objects [i]);
		dgclass->field_objects [i] = obj;

		if (inflated_field) {
			g_free (inflated_field);
		} else {
			dgclass->fields [i].name = g_strdup (dgclass->fields [i].name);
		}
	}

	dgclass->initialized = TRUE;