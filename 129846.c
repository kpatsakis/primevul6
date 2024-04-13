verifier_inflate_type (VerifyContext *ctx, MonoType *type, MonoGenericContext *context)
{
	MonoError error;
	MonoType *result;

	result = mono_class_inflate_generic_type_checked (type, context, &error);
	if (!mono_error_ok (&error)) {
		mono_error_cleanup (&error);
		return NULL;
	}
	return result;
}