is_field_on_inst (MonoClassField *field)
{
	return (field->parent->generic_class && field->parent->generic_class->is_dynamic && ((MonoDynamicGenericClass*)field->parent->generic_class)->fields);
}