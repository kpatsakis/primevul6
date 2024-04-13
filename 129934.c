mono_class_is_valid_generic_instantiation (VerifyContext *ctx, MonoClass *klass)
{
	MonoGenericClass *gklass = klass->generic_class;
	MonoGenericInst *ginst = gklass->context.class_inst;
	MonoGenericContainer *gc = gklass->container_class->generic_container;
	if (ctx && !is_valid_generic_instantiation_in_context (ctx, ginst))
		return FALSE;
	return is_valid_generic_instantiation (gc, &gklass->context, ginst);
}