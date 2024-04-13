void
mono_reflection_register_with_runtime (MonoReflectionType *type)
{
	MonoType *res = mono_reflection_type_get_handle (type);
	MonoDomain *domain = mono_object_domain ((MonoObject*)type);
	MonoClass *class;

	if (!res)
		mono_raise_exception (mono_get_exception_argument (NULL, "Invalid generic instantiation, one or more arguments are not proper user types"));

	class = mono_class_from_mono_type (res);

	mono_loader_lock (); /*same locking as mono_type_get_object*/
	mono_domain_lock (domain);

	if (!class->image->dynamic) {
		mono_class_setup_supertypes (class);
	} else {
		if (!domain->type_hash)
			domain->type_hash = mono_g_hash_table_new_type ((GHashFunc)mymono_metadata_type_hash, 
					(GCompareFunc)mymono_metadata_type_equal, MONO_HASH_VALUE_GC);
		mono_g_hash_table_insert (domain->type_hash, res, type);
	}
	mono_domain_unlock (domain);
	mono_loader_unlock ();