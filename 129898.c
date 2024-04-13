do_ldstr (VerifyContext *ctx, guint32 token)
{
	GSList *error = NULL;
	if (mono_metadata_token_code (token) != MONO_TOKEN_STRING) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid string token %x at 0x%04x", token, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return;
	}

	if (!ctx->image->dynamic && !mono_verifier_verify_string_signature (ctx->image, mono_metadata_token_index (token), &error)) {
		if (error)
			ctx->list = g_slist_concat (ctx->list, error);
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid string index %x at 0x%04x", token, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return;
	}

	if (check_overflow (ctx))
		stack_push_val (ctx, TYPE_COMPLEX,  &mono_defaults.string_class->byval_arg);
}