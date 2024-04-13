mymono_metadata_type_hash (MonoType *t1)
{
	guint hash;

	hash = t1->type;

	hash |= t1->byref << 6; /* do not collide with t1->type values */
	switch (t1->type) {
	case MONO_TYPE_VALUETYPE:
	case MONO_TYPE_CLASS:
	case MONO_TYPE_SZARRAY:
		/* check if the distribution is good enough */
		return ((hash << 5) - hash) ^ mono_aligned_addr_hash (t1->data.klass);
	case MONO_TYPE_PTR:
		return ((hash << 5) - hash) ^ mymono_metadata_type_hash (t1->data.type);
	case MONO_TYPE_GENERICINST: {
		int i;
		MonoGenericInst *inst = t1->data.generic_class->context.class_inst;
		hash += g_str_hash (t1->data.generic_class->container_class->name);
		hash *= 13;
		for (i = 0; i < inst->type_argc; ++i) {
			hash += mymono_metadata_type_hash (inst->type_argv [i]);
			hash *= 13;
		}
		return hash;
	}
	case MONO_TYPE_VAR:
	case MONO_TYPE_MVAR:
		return ((hash << 5) - hash) ^ GPOINTER_TO_UINT (t1->data.generic_param);
	}
	return hash;
}