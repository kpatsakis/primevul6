mono_param_get_objects (MonoDomain *domain, MonoMethod *method)
{
	return mono_param_get_objects_internal (domain, method, NULL);
}