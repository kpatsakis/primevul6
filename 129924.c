static gboolean
is_sre_method_on_tb_inst (MonoClass *class)
{
	check_corlib_type_cached (class, "System.Reflection.Emit", "MethodOnTypeBuilderInst");