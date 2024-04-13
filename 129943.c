static MonoMethod*
ctorbuilder_to_mono_method (MonoClass *klass, MonoReflectionCtorBuilder* mb)
{
	ReflectionMethodBuilder rmb;
	MonoMethodSignature *sig;

	mono_loader_lock ();
	sig = ctor_builder_to_signature (klass->image, mb);
	mono_loader_unlock ();

	reflection_methodbuilder_from_ctor_builder (&rmb, mb);

	mb->mhandle = reflection_methodbuilder_to_mono_method (klass, &rmb, sig);
	mono_save_custom_attrs (klass->image, mb->mhandle, mb->cattrs);

	/* If we are in a generic class, we might be called multiple times from inflate_method */
	if (!((MonoDynamicImage*)(MonoDynamicImage*)klass->image)->save && !klass->generic_container) {
		/* ilgen is no longer needed */
		mb->ilgen = NULL;
	}

	return mb->mhandle;