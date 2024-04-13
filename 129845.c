static gboolean
is_sr_mono_property (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection", "MonoProperty");