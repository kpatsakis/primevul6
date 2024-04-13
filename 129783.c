reflection_methodbuilder_from_ctor_builder (ReflectionMethodBuilder *rmb, MonoReflectionCtorBuilder *mb)
{
	const char *name = mb->attrs & METHOD_ATTRIBUTE_STATIC ? ".cctor": ".ctor";

	memset (rmb, 0, sizeof (ReflectionMethodBuilder));

	rmb->ilgen = mb->ilgen;
	rmb->rtype = mono_type_get_object (mono_domain_get (), &mono_defaults.void_class->byval_arg);
	rmb->parameters = mb->parameters;
	rmb->generic_params = NULL;
	rmb->generic_container = NULL;
	rmb->opt_types = NULL;
	rmb->pinfo = mb->pinfo;
	rmb->attrs = mb->attrs;
	rmb->iattrs = mb->iattrs;
	rmb->call_conv = mb->call_conv;
	rmb->code = NULL;
	rmb->type = mb->type;
	rmb->name = mono_string_new (mono_domain_get (), name);
	rmb->table_idx = &mb->table_idx;
	rmb->init_locals = mb->init_locals;
	rmb->skip_visibility = FALSE;
	rmb->return_modreq = NULL;
	rmb->return_modopt = NULL;
	rmb->param_modreq = mb->param_modreq;
	rmb->param_modopt = mb->param_modopt;
	rmb->permissions = mb->permissions;
	rmb->mhandle = mb->mhandle;
	rmb->nrefs = 0;
	rmb->refs = NULL;
}