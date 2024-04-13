gboolean
mono_verifier_verify_class (MonoClass *class)
{
	/*Neither <Module>, object or ifaces have parent.*/
	if (!class->parent &&
		class != mono_defaults.object_class && 
		!MONO_CLASS_IS_INTERFACE (class) &&
		(!class->image->dynamic && class->type_token != 0x2000001)) /*<Module> is the first type in the assembly*/
		return FALSE;
	if (class->parent && MONO_CLASS_IS_INTERFACE (class->parent))
		return FALSE;
	if (class->generic_container && (class->flags & TYPE_ATTRIBUTE_LAYOUT_MASK) == TYPE_ATTRIBUTE_EXPLICIT_LAYOUT)
		return FALSE;
	if (class->generic_container && !verify_generic_parameters (class))
		return FALSE;
	if (!verify_class_for_overlapping_reference_fields (class))
		return FALSE;
	if (class->generic_class && !mono_class_is_valid_generic_instantiation (NULL, class))
		return FALSE;
	if (class->generic_class == NULL && !verify_class_fields (class))
		return FALSE;
	if (class->valuetype && !verify_valuetype_layout (class))
		return FALSE;
	if (!verify_interfaces (class))
		return FALSE;
	return TRUE;