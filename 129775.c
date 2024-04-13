mono_type_from_stack_slot (ILStackDesc *slot)
{
	if (stack_slot_is_managed_pointer (slot))
		return mono_type_get_type_byref (slot->type);
	return slot->type;
}