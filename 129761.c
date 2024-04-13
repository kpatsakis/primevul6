void
mono_reflection_initialize_generic_parameter (MonoReflectionGenericParam *gparam)
{
	MonoGenericParamFull *param;
	MonoImage *image;
	MonoClass *pklass;

	MONO_ARCH_SAVE_REGS;

	param = g_new0 (MonoGenericParamFull, 1);

	if (gparam->mbuilder) {
		if (!gparam->mbuilder->generic_container) {
			MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder *)gparam->mbuilder->type;
			MonoClass *klass = mono_class_from_mono_type (mono_reflection_type_get_handle ((MonoReflectionType*)tb));
			gparam->mbuilder->generic_container = mono_image_alloc0 (klass->image, sizeof (MonoGenericContainer));
			gparam->mbuilder->generic_container->is_method = TRUE;
			/* 
			 * Cannot set owner.method, since the MonoMethod is not created yet.
			 * Set the image field instead, so type_in_image () works.
			 */
			gparam->mbuilder->generic_container->image = klass->image;
		}
		param->param.owner = gparam->mbuilder->generic_container;
	} else if (gparam->tbuilder) {
		if (!gparam->tbuilder->generic_container) {
			MonoClass *klass = mono_class_from_mono_type (mono_reflection_type_get_handle ((MonoReflectionType*)gparam->tbuilder));
			gparam->tbuilder->generic_container = mono_image_alloc0 (klass->image, sizeof (MonoGenericContainer));
			gparam->tbuilder->generic_container->owner.klass = klass;
		}
		param->param.owner = gparam->tbuilder->generic_container;
	}

	param->info.name = mono_string_to_utf8 (gparam->name);
	param->param.num = gparam->index;

	image = &gparam->tbuilder->module->dynamic_image->image;
	pklass = mono_class_from_generic_parameter ((MonoGenericParam *) param, image, gparam->mbuilder != NULL);

	gparam->type.type = &pklass->byval_arg;

	mono_class_set_ref_info (pklass, gparam);
	mono_image_lock (image);
	image->reflection_info_unregister_classes = g_slist_prepend (image->reflection_info_unregister_classes, pklass);
	mono_image_unlock (image);