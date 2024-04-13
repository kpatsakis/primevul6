static void
typebuilder_setup_fields (MonoClass *klass, MonoError *error)
{
	MonoReflectionTypeBuilder *tb = mono_class_get_ref_info (klass);
	MonoReflectionFieldBuilder *fb;
	MonoClassField *field;
	MonoImage *image = klass->image;
	const char *p, *p2;
	int i;
	guint32 len, idx, real_size = 0;

	klass->field.count = tb->num_fields;
	klass->field.first = 0;

	mono_error_init (error);

	if (tb->class_size) {
		g_assert ((tb->packing_size & 0xfffffff0) == 0);
		klass->packing_size = tb->packing_size;
		real_size = klass->instance_size + tb->class_size;
	}

	if (!klass->field.count) {
		klass->instance_size = MAX (klass->instance_size, real_size);
		return;
	}
	
	klass->fields = image_g_new0 (image, MonoClassField, klass->field.count);
	mono_class_alloc_ext (klass);
	klass->ext->field_def_values = image_g_new0 (image, MonoFieldDefaultValue, klass->field.count);
	/*
	This is, guess what, a hack.
	The issue is that the runtime doesn't know how to setup the fields of a typebuider and crash.
	On the static path no field class is resolved, only types are built. This is the right thing to do
	but we suck.
	Setting size_inited is harmless because we're doing the same job as mono_class_setup_fields anyway.
	*/
	klass->size_inited = 1;

	for (i = 0; i < klass->field.count; ++i) {
		fb = mono_array_get (tb->fields, gpointer, i);
		field = &klass->fields [i];
		field->name = mono_string_to_utf8_image (image, fb->name, error);
		if (!mono_error_ok (error))
			return;
		if (fb->attrs) {
			field->type = mono_metadata_type_dup (klass->image, mono_reflection_type_get_handle ((MonoReflectionType*)fb->type));
			field->type->attrs = fb->attrs;
		} else {
			field->type = mono_reflection_type_get_handle ((MonoReflectionType*)fb->type);
		}
		if ((fb->attrs & FIELD_ATTRIBUTE_HAS_FIELD_RVA) && fb->rva_data)
			klass->ext->field_def_values [i].data = mono_array_addr (fb->rva_data, char, 0);
		if (fb->offset != -1)
			field->offset = fb->offset;
		field->parent = klass;
		fb->handle = field;
		mono_save_custom_attrs (klass->image, field, fb->cattrs);

		if (klass->enumtype && !(field->type->attrs & FIELD_ATTRIBUTE_STATIC)) {
			klass->cast_class = klass->element_class = mono_class_from_mono_type (field->type);
		}
		if (fb->def_value) {
			MonoDynamicImage *assembly = (MonoDynamicImage*)klass->image;
			field->type->attrs |= FIELD_ATTRIBUTE_HAS_DEFAULT;
			idx = encode_constant (assembly, fb->def_value, &klass->ext->field_def_values [i].def_type);
			/* Copy the data from the blob since it might get realloc-ed */
			p = assembly->blob.data + idx;
			len = mono_metadata_decode_blob_size (p, &p2);
			len += p2 - p;
			klass->ext->field_def_values [i].data = mono_image_alloc (image, len);
			memcpy ((gpointer)klass->ext->field_def_values [i].data, p, len);
		}
	}

	klass->instance_size = MAX (klass->instance_size, real_size);
	mono_class_layout_fields (klass);