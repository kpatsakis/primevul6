static gboolean
is_usertype (MonoReflectionType *ref)
{
	MonoClass *class = mono_object_class (ref);
	return class->image != mono_defaults.corlib || strcmp ("TypeDelegator", class->name) == 0;