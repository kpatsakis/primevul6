static gboolean
is_sre_type_builder (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection.Emit", "TypeBuilder");