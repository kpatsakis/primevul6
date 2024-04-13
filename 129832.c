void
mono_reflection_setup_internal_class (MonoReflectionTypeBuilder *tb)
{
	MonoError error;
	MonoClass *klass, *parent;

	MONO_ARCH_SAVE_REGS;

	RESOLVE_TYPE (tb->parent);

	mono_loader_lock ();

	if (tb->parent) {
		/* check so we can compile corlib correctly */
		if (strcmp (mono_object_class (tb->parent)->name, "TypeBuilder") == 0) {
			/* mono_class_setup_mono_type () guaranteess type->data.klass is valid */
			parent = mono_reflection_type_get_handle ((MonoReflectionType*)tb->parent)->data.klass;
		} else {
			parent = mono_class_from_mono_type (mono_reflection_type_get_handle ((MonoReflectionType*)tb->parent));
		}
	} else {
		parent = NULL;
	}
	
	/* the type has already being created: it means we just have to change the parent */
	if (tb->type.type) {
		klass = mono_class_from_mono_type (tb->type.type);
		klass->parent = NULL;
		/* fool mono_class_setup_parent */
		klass->supertypes = NULL;
		mono_class_setup_parent (klass, parent);
		mono_class_setup_mono_type (klass);
		mono_loader_unlock ();
		return;
	}

	klass = mono_image_alloc0 (&tb->module->dynamic_image->image, sizeof (MonoClass));

	klass->image = &tb->module->dynamic_image->image;

	klass->inited = 1; /* we lie to the runtime */
	klass->name = mono_string_to_utf8_image (klass->image, tb->name, &error);
	if (!mono_error_ok (&error))
		goto failure;
	klass->name_space = mono_string_to_utf8_image (klass->image, tb->nspace, &error);
	if (!mono_error_ok (&error))
		goto failure;
	klass->type_token = MONO_TOKEN_TYPE_DEF | tb->table_idx;
	klass->flags = tb->attrs;
	
	mono_profiler_class_event (klass, MONO_PROFILE_START_LOAD);

	klass->element_class = klass;

	if (mono_class_get_ref_info (klass) == NULL) {

		mono_class_set_ref_info (klass, tb);

		/* Put into cache so mono_class_get () will find it.
		Skip nested types as those should not be available on the global scope. */
		if (!tb->nesting_type) {
			mono_image_add_to_name_cache (klass->image, klass->name_space, klass->name, tb->table_idx);
		} else {
			klass->image->reflection_info_unregister_classes =
				g_slist_prepend (klass->image->reflection_info_unregister_classes, klass);
		}
	} else {
		g_assert (mono_class_get_ref_info (klass) == tb);
	}

	mono_g_hash_table_insert (tb->module->dynamic_image->tokens,
		GUINT_TO_POINTER (MONO_TOKEN_TYPE_DEF | tb->table_idx), tb);

	if (parent != NULL) {
		mono_class_setup_parent (klass, parent);
	} else if (strcmp (klass->name, "Object") == 0 && strcmp (klass->name_space, "System") == 0) {
		const char *old_n = klass->name;
		/* trick to get relative numbering right when compiling corlib */
		klass->name = "BuildingObject";
		mono_class_setup_parent (klass, mono_defaults.object_class);
		klass->name = old_n;
	}

	if ((!strcmp (klass->name, "ValueType") && !strcmp (klass->name_space, "System")) ||
			(!strcmp (klass->name, "Object") && !strcmp (klass->name_space, "System")) ||
			(!strcmp (klass->name, "Enum") && !strcmp (klass->name_space, "System"))) {
		klass->instance_size = sizeof (MonoObject);
		klass->size_inited = 1;
		mono_class_setup_vtable_general (klass, NULL, 0);
	}

	mono_class_setup_mono_type (klass);

	mono_class_setup_supertypes (klass);

	/*
	 * FIXME: handle interfaces.
	 */

	tb->type.type = &klass->byval_arg;

	if (tb->nesting_type) {
		g_assert (tb->nesting_type->type);
		klass->nested_in = mono_class_from_mono_type (mono_reflection_type_get_handle (tb->nesting_type));
	}

	/*g_print ("setup %s as %s (%p)\n", klass->name, ((MonoObject*)tb)->vtable->klass->name, tb);*/

	mono_profiler_class_loaded (klass, MONO_PROFILE_OK);
	
	mono_loader_unlock ();
	return;

failure:
	mono_loader_unlock ();
	mono_error_raise_exception (&error);