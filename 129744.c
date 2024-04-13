static gboolean
remove_instantiations_of_and_ensure_contents (gpointer key,
						  gpointer value,
						  gpointer user_data)
{
	MonoType *type = (MonoType*)key;
	MonoClass *klass = (MonoClass*)user_data;

	if ((type->type == MONO_TYPE_GENERICINST) && (type->data.generic_class->container_class == klass)) {
		fix_partial_generic_class (mono_class_from_mono_type (type)); //Ensure it's safe to use it.
		return TRUE;
	} else
		return FALSE;