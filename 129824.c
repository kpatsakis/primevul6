stack_slot_is_complex_type_not_reference_type (ILStackDesc *slot)
{
	return stack_slot_get_type (slot) == TYPE_COMPLEX && !MONO_TYPE_IS_REFERENCE (slot->type) && !stack_slot_is_boxed_value (slot);
}