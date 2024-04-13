gboolean
mono_reflection_is_valid_dynamic_token (MonoDynamicImage *image, guint32 token)
{
	return mono_g_hash_table_lookup (image->tokens, GUINT_TO_POINTER (token)) != NULL;