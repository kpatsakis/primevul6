encode_generic_class (MonoDynamicImage *assembly, MonoGenericClass *gclass, SigBuffer *buf)
{
	int i;
	MonoGenericInst *class_inst;
	MonoClass *klass;

	g_assert (gclass);

	class_inst = gclass->context.class_inst;

	sigbuffer_add_value (buf, MONO_TYPE_GENERICINST);
	klass = gclass->container_class;
	sigbuffer_add_value (buf, klass->byval_arg.type);
	sigbuffer_add_value (buf, mono_image_typedef_or_ref_full (assembly, &klass->byval_arg, FALSE));

	sigbuffer_add_value (buf, class_inst->type_argc);
	for (i = 0; i < class_inst->type_argc; ++i)
		encode_type (assembly, class_inst->type_argv [i], buf);

}