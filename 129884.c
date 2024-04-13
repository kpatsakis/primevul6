store_local (VerifyContext *ctx, guint32 arg)
{
	ILStackDesc *value;
	if (arg >= ctx->num_locals) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Method doesn't have local var %d at 0x%04x", arg + 1, ctx->ip_offset));
		return;
	}

	/*TODO verify definite assigment */		
	if (check_underflow (ctx, 1)) {
		value = stack_pop(ctx);
		if (!verify_stack_type_compatibility (ctx, ctx->locals [arg], value)) {
			char *expected = mono_type_full_name (ctx->locals [arg]);
			char *found = stack_slot_full_name (value);
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Incompatible type '%s' on stack cannot be stored to local %d with type '%s' at 0x%04x",
					found,
					arg,
					expected,
					ctx->ip_offset));
			g_free (expected);
			g_free (found);	
		}
	}
}