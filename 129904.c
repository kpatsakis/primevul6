mono_reflection_get_custom_attrs_info (MonoObject *obj)
{
	MonoClass *klass;
	MonoCustomAttrInfo *cinfo = NULL;
	
	klass = obj->vtable->klass;
	if (klass == mono_defaults.monotype_class) {
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType *)obj);
		klass = mono_class_from_mono_type (type);
		/*We cannot mono_class_init the class from which we'll load the custom attributes since this must work with broken types.*/
		cinfo = mono_custom_attrs_from_class (klass);
	} else if (strcmp ("Assembly", klass->name) == 0 || strcmp ("MonoAssembly", klass->name) == 0) {
		MonoReflectionAssembly *rassembly = (MonoReflectionAssembly*)obj;
		cinfo = mono_custom_attrs_from_assembly (rassembly->assembly);
	} else if (strcmp ("Module", klass->name) == 0 || strcmp ("MonoModule", klass->name) == 0) {
		MonoReflectionModule *module = (MonoReflectionModule*)obj;
		cinfo = mono_custom_attrs_from_module (module->image);
	} else if (strcmp ("MonoProperty", klass->name) == 0) {
		MonoReflectionProperty *rprop = (MonoReflectionProperty*)obj;
		cinfo = mono_custom_attrs_from_property (rprop->property->parent, rprop->property);
	} else if (strcmp ("MonoEvent", klass->name) == 0) {
		MonoReflectionMonoEvent *revent = (MonoReflectionMonoEvent*)obj;
		cinfo = mono_custom_attrs_from_event (revent->event->parent, revent->event);
	} else if (strcmp ("MonoField", klass->name) == 0) {
		MonoReflectionField *rfield = (MonoReflectionField*)obj;
		cinfo = mono_custom_attrs_from_field (rfield->field->parent, rfield->field);
	} else if ((strcmp ("MonoMethod", klass->name) == 0) || (strcmp ("MonoCMethod", klass->name) == 0)) {
		MonoReflectionMethod *rmethod = (MonoReflectionMethod*)obj;
		cinfo = mono_custom_attrs_from_method (rmethod->method);
	} else if ((strcmp ("MonoGenericMethod", klass->name) == 0) || (strcmp ("MonoGenericCMethod", klass->name) == 0)) {
		MonoReflectionMethod *rmethod = (MonoReflectionMethod*)obj;
		cinfo = mono_custom_attrs_from_method (rmethod->method);
	} else if (strcmp ("ParameterInfo", klass->name) == 0) {
		MonoReflectionParameter *param = (MonoReflectionParameter*)obj;
		MonoClass *member_class = mono_object_class (param->MemberImpl);
		if (mono_class_is_reflection_method_or_constructor (member_class)) {
			MonoReflectionMethod *rmethod = (MonoReflectionMethod*)param->MemberImpl;
			cinfo = mono_custom_attrs_from_param (rmethod->method, param->PositionImpl + 1);
		} else if (is_sr_mono_property (member_class)) {
			MonoReflectionProperty *prop = (MonoReflectionProperty *)param->MemberImpl;
			MonoMethod *method;
			if (!(method = prop->property->get))
				method = prop->property->set;
			g_assert (method);

			cinfo = mono_custom_attrs_from_param (method, param->PositionImpl + 1);
		} 
#ifndef DISABLE_REFLECTION_EMIT
		else if (is_sre_method_on_tb_inst (member_class)) {/*XXX This is a workaround for Compiler Context*/
			MonoMethod *method = mono_reflection_method_on_tb_inst_get_handle ((MonoReflectionMethodOnTypeBuilderInst*)param->MemberImpl);
			cinfo = mono_custom_attrs_from_param (method, param->PositionImpl + 1);
		} else if (is_sre_ctor_on_tb_inst (member_class)) { /*XX This is a workaround for Compiler Context*/
			MonoReflectionCtorOnTypeBuilderInst *c = (MonoReflectionCtorOnTypeBuilderInst*)param->MemberImpl;
			MonoMethod *method = NULL;
			if (is_sre_ctor_builder (mono_object_class (c->cb)))
				method = ((MonoReflectionCtorBuilder *)c->cb)->mhandle;
			else if (is_sr_mono_cmethod (mono_object_class (c->cb)))
				method = ((MonoReflectionMethod *)c->cb)->method;
			else
				g_error ("mono_reflection_get_custom_attrs_info:: can't handle a CTBI with base_method of type %s", mono_type_get_full_name (member_class));

			cinfo = mono_custom_attrs_from_param (method, param->PositionImpl + 1);
		} 
#endif
		else {
			char *type_name = mono_type_get_full_name (member_class);
			char *msg = g_strdup_printf ("Custom attributes on a ParamInfo with member %s are not supported", type_name);
			MonoException *ex = mono_get_exception_not_supported  (msg);
			g_free (type_name);
			g_free (msg);
			mono_raise_exception (ex);
		}
	} else if (strcmp ("AssemblyBuilder", klass->name) == 0) {
		MonoReflectionAssemblyBuilder *assemblyb = (MonoReflectionAssemblyBuilder*)obj;
		cinfo = mono_custom_attrs_from_builders (NULL, assemblyb->assembly.assembly->image, assemblyb->cattrs);
	} else if (strcmp ("TypeBuilder", klass->name) == 0) {
		MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder*)obj;
		cinfo = mono_custom_attrs_from_builders (NULL, &tb->module->dynamic_image->image, tb->cattrs);
	} else if (strcmp ("ModuleBuilder", klass->name) == 0) {
		MonoReflectionModuleBuilder *mb = (MonoReflectionModuleBuilder*)obj;
		cinfo = mono_custom_attrs_from_builders (NULL, &mb->dynamic_image->image, mb->cattrs);
	} else if (strcmp ("ConstructorBuilder", klass->name) == 0) {
		MonoReflectionCtorBuilder *cb = (MonoReflectionCtorBuilder*)obj;
		cinfo = mono_custom_attrs_from_builders (NULL, cb->mhandle->klass->image, cb->cattrs);
	} else if (strcmp ("MethodBuilder", klass->name) == 0) {
		MonoReflectionMethodBuilder *mb = (MonoReflectionMethodBuilder*)obj;
		cinfo = mono_custom_attrs_from_builders (NULL, mb->mhandle->klass->image, mb->cattrs);
	} else if (strcmp ("FieldBuilder", klass->name) == 0) {
		MonoReflectionFieldBuilder *fb = (MonoReflectionFieldBuilder*)obj;
		cinfo = mono_custom_attrs_from_builders (NULL, &((MonoReflectionTypeBuilder*)fb->typeb)->module->dynamic_image->image, fb->cattrs);
	} else if (strcmp ("MonoGenericClass", klass->name) == 0) {
		MonoReflectionGenericClass *gclass = (MonoReflectionGenericClass*)obj;
		cinfo = mono_reflection_get_custom_attrs_info ((MonoObject*)gclass->generic_type);
	} else { /* handle other types here... */
		g_error ("get custom attrs not yet supported for %s", klass->name);
	}

	return cinfo;
}