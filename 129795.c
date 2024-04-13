static gboolean
mono_opcode_is_prefix (int op)
{
	switch (op) {
	case MONO_CEE_UNALIGNED_:
	case MONO_CEE_VOLATILE_:
	case MONO_CEE_TAIL_:
	case MONO_CEE_CONSTRAINED_:
	case MONO_CEE_READONLY_:
		return TRUE;
	}
	return FALSE;