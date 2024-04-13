static MonoMethod*
reflection_methodbuilder_to_mono_method (MonoClass *klass,
					 ReflectionMethodBuilder *rmb,
					 MonoMethodSignature *sig)
{
	MonoError error;
	MonoMethod *m;
	MonoMethodWrapper *wrapperm;
	MonoMarshalSpec **specs;
	MonoReflectionMethodAux *method_aux;
	MonoImage *image;
	gboolean dynamic;
	int i;

	mono_error_init (&error);
	/*
	 * Methods created using a MethodBuilder should have their memory allocated
	 * inside the image mempool, while dynamic methods should have their memory
	 * malloc'd.
	 */
	dynamic = rmb->refs != NULL;
	image = dynamic ? NULL : klass->image;

	if (!dynamic)
		g_assert (!klass->generic_class);

	mono_loader_lock ();

	if ((rmb->attrs & METHOD_ATTRIBUTE_PINVOKE_IMPL) ||
			(rmb->iattrs & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL))
		m = (MonoMethod *)image_g_new0 (image, MonoMethodPInvoke, 1);
	else
		m = (MonoMethod *)image_g_new0 (image, MonoMethodWrapper, 1);

	wrapperm = (MonoMethodWrapper*)m;

	m->dynamic = dynamic;
	m->slot = -1;
	m->flags = rmb->attrs;
	m->iflags = rmb->iattrs;
	m->name = mono_string_to_utf8_image (image, rmb->name, &error);
	g_assert (mono_error_ok (&error));
	m->klass = klass;
	m->signature = sig;
	m->sre_method = TRUE;
	m->skip_visibility = rmb->skip_visibility;
	if (rmb->table_idx)
		m->token = MONO_TOKEN_METHOD_DEF | (*rmb->table_idx);

	if (m->iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL) {
		if (klass == mono_defaults.string_class && !strcmp (m->name, ".ctor"))
			m->string_ctor = 1;

		m->signature->pinvoke = 1;
	} else if (m->flags & METHOD_ATTRIBUTE_PINVOKE_IMPL) {
		m->signature->pinvoke = 1;

		method_aux = image_g_new0 (image, MonoReflectionMethodAux, 1);

		method_aux->dllentry = rmb->dllentry ? mono_string_to_utf8_image (image, rmb->dllentry, &error) : image_strdup (image, m->name);
		g_assert (mono_error_ok (&error));
		method_aux->dll = mono_string_to_utf8_image (image, rmb->dll, &error);
		g_assert (mono_error_ok (&error));
		
		((MonoMethodPInvoke*)m)->piflags = (rmb->native_cc << 8) | (rmb->charset ? (rmb->charset - 1) * 2 : 0) | rmb->extra_flags;

		if (klass->image->dynamic)
			g_hash_table_insert (((MonoDynamicImage*)klass->image)->method_aux_hash, m, method_aux);

		mono_loader_unlock ();

		return m;
	} else if (!(m->flags & METHOD_ATTRIBUTE_ABSTRACT) &&
			   !(m->iflags & METHOD_IMPL_ATTRIBUTE_RUNTIME)) {
		MonoMethodHeader *header;
		guint32 code_size;
		gint32 max_stack, i;
		gint32 num_locals = 0;
		gint32 num_clauses = 0;
		guint8 *code;

		if (rmb->ilgen) {
			code = mono_array_addr (rmb->ilgen->code, guint8, 0);
			code_size = rmb->ilgen->code_len;
			max_stack = rmb->ilgen->max_stack;
			num_locals = rmb->ilgen->locals ? mono_array_length (rmb->ilgen->locals) : 0;
			if (rmb->ilgen->ex_handlers)
				num_clauses = method_count_clauses (rmb->ilgen);
		} else {
			if (rmb->code) {
				code = mono_array_addr (rmb->code, guint8, 0);
				code_size = mono_array_length (rmb->code);
				/* we probably need to run a verifier on the code... */
				max_stack = 8; 
			}
			else {
				code = NULL;
				code_size = 0;
				max_stack = 8;
			}
		}

		header = image_g_malloc0 (image, MONO_SIZEOF_METHOD_HEADER + num_locals * sizeof (MonoType*));
		header->code_size = code_size;
		header->code = image_g_malloc (image, code_size);
		memcpy ((char*)header->code, code, code_size);
		header->max_stack = max_stack;
		header->init_locals = rmb->init_locals;
		header->num_locals = num_locals;

		for (i = 0; i < num_locals; ++i) {
			MonoReflectionLocalBuilder *lb = 
				mono_array_get (rmb->ilgen->locals, MonoReflectionLocalBuilder*, i);

			header->locals [i] = image_g_new0 (image, MonoType, 1);
			memcpy (header->locals [i], mono_reflection_type_get_handle ((MonoReflectionType*)lb->type), MONO_SIZEOF_TYPE);
		}

		header->num_clauses = num_clauses;
		if (num_clauses) {
			header->clauses = method_encode_clauses (image, (MonoDynamicImage*)klass->image,
				 rmb->ilgen, num_clauses);
		}

		wrapperm->header = header;
	}

	if (rmb->generic_params) {
		int count = mono_array_length (rmb->generic_params);
		MonoGenericContainer *container = rmb->generic_container;

		g_assert (container);

		container->type_argc = count;
		container->type_params = image_g_new0 (image, MonoGenericParamFull, count);
		container->owner.method = m;

		m->is_generic = TRUE;
		mono_method_set_generic_container (m, container);

		for (i = 0; i < count; i++) {
			MonoReflectionGenericParam *gp =
				mono_array_get (rmb->generic_params, MonoReflectionGenericParam*, i);
			MonoGenericParamFull *param = (MonoGenericParamFull *) mono_reflection_type_get_handle ((MonoReflectionType*)gp)->data.generic_param;
			container->type_params [i] = *param;
		}

		/*
		 * The method signature might have pointers to generic parameters that belong to other methods.
		 * This is a valid SRE case, but the resulting method signature must be encoded using the proper
		 * generic parameters.
		 */
		for (i = 0; i < m->signature->param_count; ++i) {
			MonoType *t = m->signature->params [i];
			if (t->type == MONO_TYPE_MVAR) {
				MonoGenericParam *gparam =  t->data.generic_param;
				if (gparam->num < count) {
					m->signature->params [i] = mono_metadata_type_dup (image, m->signature->params [i]);
					m->signature->params [i]->data.generic_param = mono_generic_container_get_param (container, gparam->num);
				}

			}
		}

		if (klass->generic_container) {
			container->parent = klass->generic_container;
			container->context.class_inst = klass->generic_container->context.class_inst;
		}
		container->context.method_inst = mono_get_shared_generic_inst (container);
	}

	if (rmb->refs) {
		MonoMethodWrapper *mw = (MonoMethodWrapper*)m;
		int i;
		void **data;

		m->wrapper_type = MONO_WRAPPER_DYNAMIC_METHOD;

		mw->method_data = data = image_g_new (image, gpointer, rmb->nrefs + 1);
		data [0] = GUINT_TO_POINTER (rmb->nrefs);
		for (i = 0; i < rmb->nrefs; ++i)
			data [i + 1] = rmb->refs [i];
	}

	method_aux = NULL;

	/* Parameter info */
	if (rmb->pinfo) {
		if (!method_aux)
			method_aux = image_g_new0 (image, MonoReflectionMethodAux, 1);
		method_aux->param_names = image_g_new0 (image, char *, mono_method_signature (m)->param_count + 1);
		for (i = 0; i <= m->signature->param_count; ++i) {
			MonoReflectionParamBuilder *pb;
			if ((pb = mono_array_get (rmb->pinfo, MonoReflectionParamBuilder*, i))) {
				if ((i > 0) && (pb->attrs)) {
					/* Make a copy since it might point to a shared type structure */
					m->signature->params [i - 1] = mono_metadata_type_dup (klass->image, m->signature->params [i - 1]);
					m->signature->params [i - 1]->attrs = pb->attrs;
				}

				if (pb->attrs & PARAM_ATTRIBUTE_HAS_DEFAULT) {
					MonoDynamicImage *assembly;
					guint32 idx, def_type, len;
					char *p;
					const char *p2;

					if (!method_aux->param_defaults) {
						method_aux->param_defaults = image_g_new0 (image, guint8*, m->signature->param_count + 1);
						method_aux->param_default_types = image_g_new0 (image, guint32, m->signature->param_count + 1);
					}
					assembly = (MonoDynamicImage*)klass->image;
					idx = encode_constant (assembly, pb->def_value, &def_type);
					/* Copy the data from the blob since it might get realloc-ed */
					p = assembly->blob.data + idx;
					len = mono_metadata_decode_blob_size (p, &p2);
					len += p2 - p;
					method_aux->param_defaults [i] = image_g_malloc (image, len);
					method_aux->param_default_types [i] = def_type;
					memcpy ((gpointer)method_aux->param_defaults [i], p, len);
				}

				if (pb->name) {
					method_aux->param_names [i] = mono_string_to_utf8_image (image, pb->name, &error);
					g_assert (mono_error_ok (&error));
				}
				if (pb->cattrs) {
					if (!method_aux->param_cattr)
						method_aux->param_cattr = image_g_new0 (image, MonoCustomAttrInfo*, m->signature->param_count + 1);
					method_aux->param_cattr [i] = mono_custom_attrs_from_builders (image, klass->image, pb->cattrs);
				}
			}
		}
	}

	/* Parameter marshalling */
	specs = NULL;
	if (rmb->pinfo)		
		for (i = 0; i < mono_array_length (rmb->pinfo); ++i) {
			MonoReflectionParamBuilder *pb;
			if ((pb = mono_array_get (rmb->pinfo, MonoReflectionParamBuilder*, i))) {
				if (pb->marshal_info) {
					if (specs == NULL)
						specs = image_g_new0 (image, MonoMarshalSpec*, sig->param_count + 1);
					specs [pb->position] = 
						mono_marshal_spec_from_builder (image, klass->image->assembly, pb->marshal_info);
				}
			}
		}
	if (specs != NULL) {
		if (!method_aux)
			method_aux = image_g_new0 (image, MonoReflectionMethodAux, 1);
		method_aux->param_marshall = specs;
	}

	if (klass->image->dynamic && method_aux)
		g_hash_table_insert (((MonoDynamicImage*)klass->image)->method_aux_hash, m, method_aux);

	mono_loader_unlock ();

	return m;