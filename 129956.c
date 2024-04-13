guint32
mono_declsec_flags_from_class (MonoClass *klass)
{
	if (klass->flags & TYPE_ATTRIBUTE_HAS_SECURITY) {
		if (!klass->ext || !klass->ext->declsec_flags) {
			guint32 idx;

			idx = mono_metadata_token_index (klass->type_token);
			idx <<= MONO_HAS_DECL_SECURITY_BITS;
			idx |= MONO_HAS_DECL_SECURITY_TYPEDEF;
			mono_loader_lock ();
			mono_class_alloc_ext (klass);
			mono_loader_unlock ();
			/* we cache the flags on classes */
			klass->ext->declsec_flags = mono_declsec_get_flags (klass->image, idx);
		}
		return klass->ext->declsec_flags;
	}
	return 0;