get_generic_param (VerifyContext *ctx, MonoType *param) 
{
	guint16 param_num = mono_type_get_generic_param_num (param);
	if (param->type == MONO_TYPE_VAR) {
		if (!ctx->generic_context->class_inst || ctx->generic_context->class_inst->type_argc <= param_num) {
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid generic type argument %d", param_num));
			return NULL;
		}
		return ctx->generic_context->class_inst->type_argv [param_num]->data.generic_param;
	}
	
	/*param must be a MVAR */
	if (!ctx->generic_context->method_inst || ctx->generic_context->method_inst->type_argc <= param_num) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid generic method argument %d", param_num));
		return NULL;
	}
	return ctx->generic_context->method_inst->type_argv [param_num]->data.generic_param;
	
}