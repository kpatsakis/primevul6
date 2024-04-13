stack_slot_get_underlying_type (ILStackDesc *value)
{
	return value->stype & TYPE_MASK;
}