static int
get_field_end (MonoClassField *field)
{
	int align;
	int size = mono_type_size (field->type, &align);
	if (size == 0)
		size = 4; /*FIXME Is this a safe bet?*/
	return size + field->offset;