static MonoBoolean
mono_declsec_get_class_demands_params (MonoClass *klass, MonoDeclSecurityActions* demands, 
	guint32 id_std, guint32 id_noncas, guint32 id_choice)
{
	guint32 idx = mono_metadata_token_index (klass->type_token);
	idx <<= MONO_HAS_DECL_SECURITY_BITS;
	idx |= MONO_HAS_DECL_SECURITY_TYPEDEF;
	return fill_actions_from_index (klass->image, idx, demands, id_std, id_noncas, id_choice);