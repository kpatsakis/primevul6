static void
fix_partial_generic_class (MonoClass *klass)
{
	MonoClass *gklass = klass->generic_class->container_class;
	MonoDynamicGenericClass *dgclass;
	int i;

	if (klass->wastypebuilder)
		return;

	dgclass = (MonoDynamicGenericClass *)  klass->generic_class;
	if (klass->parent != gklass->parent) {
		MonoError error;
		MonoType *parent_type = mono_class_inflate_generic_type_checked (&gklass->parent->byval_arg, &klass->generic_class->context, &error);
		if (mono_error_ok (&error)) {
			MonoClass *parent = mono_class_from_mono_type (parent_type);
			mono_metadata_free_type (parent_type);
			if (parent != klass->parent) {
				/*fool mono_class_setup_parent*/
				klass->supertypes = NULL;
				mono_class_setup_parent (klass, parent);
			}
		} else {
			mono_class_set_failure (klass, MONO_EXCEPTION_TYPE_LOAD, NULL);
			mono_error_cleanup (&error);
			if (gklass->wastypebuilder)
				klass->wastypebuilder = TRUE;
			return;
		}
	}

	if (!dgclass->initialized)
		return;

	if (klass->method.count != gklass->method.count) {
		klass->method.count = gklass->method.count;
		klass->methods = mono_image_alloc (klass->image, sizeof (MonoMethod*) * (klass->method.count + 1));

		for (i = 0; i < klass->method.count; i++) {
			klass->methods [i] = mono_class_inflate_generic_method_full (
				gklass->methods [i], klass, mono_class_get_context (klass));
		}
	}

	if (klass->interface_count && klass->interface_count != gklass->interface_count) {
		klass->interface_count = gklass->interface_count;
		klass->interfaces = mono_image_alloc (klass->image, sizeof (MonoClass*) * gklass->interface_count);
		klass->interfaces_packed = NULL; /*make setup_interface_offsets happy*/

		for (i = 0; i < gklass->interface_count; ++i) {
			MonoType *iface_type = mono_class_inflate_generic_type (&gklass->interfaces [i]->byval_arg, mono_class_get_context (klass));
			klass->interfaces [i] = mono_class_from_mono_type (iface_type);
			mono_metadata_free_type (iface_type);

			ensure_runtime_vtable (klass->interfaces [i]);
		}
		klass->interfaces_inited = 1;
	}

	if (klass->field.count != gklass->field.count) {
		klass->field.count = gklass->field.count;
		klass->fields = image_g_new0 (klass->image, MonoClassField, klass->field.count);

		for (i = 0; i < klass->field.count; i++) {
			klass->fields [i] = gklass->fields [i];
			klass->fields [i].parent = klass;
			klass->fields [i].type = mono_class_inflate_generic_type (gklass->fields [i].type, mono_class_get_context (klass));
		}
	}

	/*We can only finish with this klass once it's parent has as well*/
	if (gklass->wastypebuilder)
		klass->wastypebuilder = TRUE;
	return;