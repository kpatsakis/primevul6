mono_type_normalize (MonoType *type)
{
	int i;
	MonoGenericClass *gclass;
	MonoGenericInst *ginst;
	MonoClass *gtd;
	MonoGenericContainer *gcontainer;
	MonoType **argv = NULL;
	gboolean is_denorm_gtd = TRUE, requires_rebind = FALSE;

	if (type->type != MONO_TYPE_GENERICINST)
		return type;

	gclass = type->data.generic_class;
	ginst = gclass->context.class_inst;
	if (!ginst->is_open)
		return type;

	gtd = gclass->container_class;
	gcontainer = gtd->generic_container;
	argv = g_newa (MonoType*, ginst->type_argc);

	for (i = 0; i < ginst->type_argc; ++i) {
		MonoType *t = ginst->type_argv [i], *norm;
		if (t->type != MONO_TYPE_VAR || t->data.generic_param->num != i || t->data.generic_param->owner != gcontainer)
			is_denorm_gtd = FALSE;
		norm = mono_type_normalize (t);
		argv [i] = norm;
		if (norm != t)
			requires_rebind = TRUE;
	}

	if (is_denorm_gtd)
		return type->byref == gtd->byval_arg.byref ? &gtd->byval_arg : &gtd->this_arg;

	if (requires_rebind) {
		MonoClass *klass = mono_class_bind_generic_parameters (gtd, ginst->type_argc, argv, gclass->is_dynamic);
		return type->byref == klass->byval_arg.byref ? &klass->byval_arg : &klass->this_arg;
	}

	return type;
}