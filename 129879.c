do_conversion (VerifyContext *ctx, int kind) 
{
	ILStackDesc *value;
	if (!check_underflow (ctx, 1))
		return;
	value = stack_pop (ctx);

	switch (stack_slot_get_type (value)) {
	case TYPE_I4:
	case TYPE_I8:
	case TYPE_NATIVE_INT:
	case TYPE_R8:
		break;
	default:
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid type (%s) at stack for conversion operation. Numeric type expected at 0x%04x", stack_slot_get_name (value), ctx->ip_offset));
	}

	switch (kind) {
	case TYPE_I4:
		stack_push_val (ctx, TYPE_I4, &mono_defaults.int32_class->byval_arg);
		break;
	case TYPE_I8:
		stack_push_val (ctx,TYPE_I8, &mono_defaults.int64_class->byval_arg);
		break;
	case TYPE_R8:
		stack_push_val (ctx, TYPE_R8, &mono_defaults.double_class->byval_arg);
		break;
	case TYPE_NATIVE_INT:
		stack_push_val (ctx, TYPE_NATIVE_INT, &mono_defaults.int_class->byval_arg);
		break;
	default:
		g_error ("unknown type %02x in conversion", kind);

	}
}