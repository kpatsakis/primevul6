static gboolean
is_sre_pointer (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection.Emit", "PointerType");