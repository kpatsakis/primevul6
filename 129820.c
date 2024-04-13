static void
ensure_runtime_vtable (MonoClass *klass)
{
	MonoReflectionTypeBuilder *tb = mono_class_get_ref_info (klass);
	int i, num, j;

	if (!klass->image->dynamic || (!tb && !klass->generic_class) || klass->wastypebuilder)
		return;
	if (klass->parent)
		ensure_runtime_vtable (klass->parent);

	if (tb) {
		num = tb->ctors? mono_array_length (tb->ctors): 0;
		num += tb->num_methods;
		klass->method.count = num;
		klass->methods = mono_image_alloc (klass->image, sizeof (MonoMethod*) * num);
		num = tb->ctors? mono_array_length (tb->ctors): 0;
		for (i = 0; i < num; ++i)
			klass->methods [i] = ctorbuilder_to_mono_method (klass, mono_array_get (tb->ctors, MonoReflectionCtorBuilder*, i));
		num = tb->num_methods;
		j = i;
		for (i = 0; i < num; ++i)
			klass->methods [j++] = methodbuilder_to_mono_method (klass, mono_array_get (tb->methods, MonoReflectionMethodBuilder*, i));
	
		if (tb->interfaces) {
			klass->interface_count = mono_array_length (tb->interfaces);
			klass->interfaces = mono_image_alloc (klass->image, sizeof (MonoClass*) * klass->interface_count);
			for (i = 0; i < klass->interface_count; ++i) {
				MonoType *iface = mono_type_array_get_and_resolve (tb->interfaces, i);
				klass->interfaces [i] = mono_class_from_mono_type (iface);
				ensure_runtime_vtable (klass->interfaces [i]);
			}
			klass->interfaces_inited = 1;
		}
	} else if (klass->generic_class){
		ensure_generic_class_runtime_vtable (klass);
	}

	if (klass->flags & TYPE_ATTRIBUTE_INTERFACE) {
		for (i = 0; i < klass->method.count; ++i)
			klass->methods [i]->slot = i;
		
		klass->interfaces_packed = NULL; /*make setup_interface_offsets happy*/
		mono_class_setup_interface_offsets (klass);
		mono_class_setup_interface_id (klass);
	}

	/*
	 * The generic vtable is needed even if image->run is not set since some
	 * runtime code like ves_icall_Type_GetMethodsByName depends on 
	 * method->slot being defined.
	 */

	/* 
	 * tb->methods could not be freed since it is used for determining 
	 * overrides during dynamic vtable construction.
	 */