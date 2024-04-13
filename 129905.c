void
mono_reflection_get_dynamic_overrides (MonoClass *klass, MonoMethod ***overrides, int *num_overrides)
{
	MonoReflectionTypeBuilder *tb;
	int i, onum;

	*overrides = NULL;
	*num_overrides = 0;

	g_assert (klass->image->dynamic);

	if (!mono_class_get_ref_info (klass))
		return;

	g_assert (strcmp (((MonoObject*)mono_class_get_ref_info (klass))->vtable->klass->name, "TypeBuilder") == 0);

	tb = (MonoReflectionTypeBuilder*)mono_class_get_ref_info (klass);

	onum = 0;
	if (tb->methods) {
		for (i = 0; i < tb->num_methods; ++i) {
			MonoReflectionMethodBuilder *mb = 
				mono_array_get (tb->methods, MonoReflectionMethodBuilder*, i);
			if (mb->override_method)
				onum ++;
		}
	}

	if (onum) {
		*overrides = g_new0 (MonoMethod*, onum * 2);

		onum = 0;
		for (i = 0; i < tb->num_methods; ++i) {
			MonoReflectionMethodBuilder *mb = 
				mono_array_get (tb->methods, MonoReflectionMethodBuilder*, i);
			if (mb->override_method) {
				(*overrides) [onum * 2] = mono_reflection_method_get_handle ((MonoObject *)mb->override_method);
				(*overrides) [onum * 2 + 1] = mb->mhandle;

				g_assert (mb->mhandle);

				onum ++;
			}
		}
	}

	*num_overrides = onum;