void
mono_reflection_create_generic_class (MonoReflectionTypeBuilder *tb)
{
	MonoClass *klass;
	int count, i;

	MONO_ARCH_SAVE_REGS;

	klass = mono_class_from_mono_type (tb->type.type);

	count = tb->generic_params ? mono_array_length (tb->generic_params) : 0;

	if (klass->generic_container || (count == 0))
		return;

	g_assert (tb->generic_container && (tb->generic_container->owner.klass == klass));

	klass->generic_container = mono_image_alloc0 (klass->image, sizeof (MonoGenericContainer));

	klass->generic_container->owner.klass = klass;
	klass->generic_container->type_argc = count;
	klass->generic_container->type_params = mono_image_alloc0 (klass->image, sizeof (MonoGenericParamFull) * count);

	klass->is_generic = 1;

	for (i = 0; i < count; i++) {
		MonoReflectionGenericParam *gparam = mono_array_get (tb->generic_params, gpointer, i);
		MonoGenericParamFull *param = (MonoGenericParamFull *) mono_reflection_type_get_handle ((MonoReflectionType*)gparam)->data.generic_param;
		klass->generic_container->type_params [i] = *param;
		/*Make sure we are a diferent type instance */
		klass->generic_container->type_params [i].param.owner = klass->generic_container;
		klass->generic_container->type_params [i].info.pklass = NULL;
		klass->generic_container->type_params [i].info.flags = gparam->attrs;

		g_assert (klass->generic_container->type_params [i].param.owner);
	}

	klass->generic_container->context.class_inst = mono_get_shared_generic_inst (klass->generic_container);