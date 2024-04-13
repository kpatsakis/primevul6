static MonoMethod*
mono_reflection_method_get_handle (MonoObject *method)
{
	MonoClass *class = mono_object_class (method);
	if (is_sr_mono_method (class) || is_sr_mono_generic_method (class)) {
		MonoReflectionMethod *sr_method = (MonoReflectionMethod*)method;
		return sr_method->method;
	}
	if (is_sre_method_builder (class)) {
		MonoReflectionMethodBuilder *mb = (MonoReflectionMethodBuilder*)method;
		return mb->mhandle;
	}
	if (is_sre_method_on_tb_inst (class)) {
		MonoReflectionMethodOnTypeBuilderInst *m = (MonoReflectionMethodOnTypeBuilderInst*)method;
		MonoMethod *result;
		/*FIXME move this to a proper method and unify with resolve_object*/
		if (m->method_args) {
			result = mono_reflection_method_on_tb_inst_get_handle (m);
		} else {
			MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType*)m->inst);
			MonoClass *inflated_klass = mono_class_from_mono_type (type);
			MonoMethod *mono_method;

			if (is_sre_method_builder (mono_object_class (m->mb)))
				mono_method = ((MonoReflectionMethodBuilder *)m->mb)->mhandle;
 			else if (is_sr_mono_method (mono_object_class (m->mb)))
				mono_method = ((MonoReflectionMethod *)m->mb)->method;
			else
				g_error ("resolve_object:: can't handle a MTBI with base_method of type %s", mono_type_get_full_name (mono_object_class (m->mb)));

			result = inflate_mono_method (inflated_klass, mono_method, (MonoObject*)m->mb);
		}
		return result;
	}

	g_error ("Can't handle methods of type %s:%s", class->name_space, class->name);
	return NULL;