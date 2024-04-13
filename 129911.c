get_boxable_mono_type (VerifyContext* ctx, int token, const char *opcode)
{
	MonoType *type;
	MonoClass *class;

	if (!(type = verifier_load_type (ctx, token, opcode)))
		return NULL;

	if (type->byref && type->type != MONO_TYPE_TYPEDBYREF) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid use of byref type for %s at 0x%04x", opcode, ctx->ip_offset));
		return NULL;
	}

	if (type->type == MONO_TYPE_VOID) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid use of void type for %s at 0x%04x", opcode, ctx->ip_offset));
		return NULL;
	}

	if (type->type == MONO_TYPE_TYPEDBYREF)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid use of typedbyref for %s at 0x%04x", opcode, ctx->ip_offset));

	if (!(class = mono_class_from_mono_type (type)))
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Could not retrieve type token for %s at 0x%04x", opcode, ctx->ip_offset));

	if (class->generic_container && type->type != MONO_TYPE_GENERICINST)
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use the generic type definition in a boxable type position for %s at 0x%04x", opcode, ctx->ip_offset));	

	check_unverifiable_type (ctx, type);
	return type;
}