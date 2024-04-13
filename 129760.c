void
mono_reflection_get_dynamic_overrides (MonoClass *klass, MonoMethod ***overrides, int *num_overrides)
{
	*overrides = NULL;
	*num_overrides = 0;