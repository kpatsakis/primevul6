stack_slot_is_managed_mutability_pointer (ILStackDesc *value)
{
	return (value->stype & CMMP_MASK) == CMMP_MASK;
}