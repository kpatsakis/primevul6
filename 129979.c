push_arg (VerifyContext *ctx, unsigned int arg, int take_addr) 
{
	ILStackDesc *top;

	if (arg >= ctx->max_args) {
		if (take_addr) 
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Method doesn't have argument %d", arg + 1));
		else {
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Method doesn't have argument %d", arg + 1));
			if (check_overflow (ctx)) //FIXME: what sane value could we ever push?
				stack_push_val (ctx, TYPE_I4, &mono_defaults.int32_class->byval_arg);
		}
	} else if (check_overflow (ctx)) {
		/*We must let the value be pushed, otherwise we would get an underflow error*/
		check_unverifiable_type (ctx, ctx->params [arg]);
		if (ctx->params [arg]->byref && take_addr)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("ByRef of ByRef at 0x%04x", ctx->ip_offset));
		top = stack_push (ctx);
		if (!set_stack_value (ctx, top, ctx->params [arg], take_addr))
			return;

		if (arg == 0 && !(ctx->method->flags & METHOD_ATTRIBUTE_STATIC)) {
			if (take_addr)
				ctx->has_this_store = TRUE;
			else
				top->stype |= THIS_POINTER_MASK;
			if (mono_method_is_constructor (ctx->method) && !ctx->super_ctor_called && !ctx->method->klass->valuetype)
				top->stype |= UNINIT_THIS_MASK;
		}
	} 
}