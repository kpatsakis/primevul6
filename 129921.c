stack_copy (ILCodeDesc *to, ILCodeDesc *from)
{
	ensure_stack_size (to, from->size);
	to->size = from->size;

	/*stack copy happens at merge points, which have small stacks*/
	if (from->size)
		memcpy (to->stack, from->stack, sizeof (ILStackDesc) * from->size);
}