mono_type_from_opcode (int opcode) {
	switch (opcode) {
	case CEE_LDIND_I1:
	case CEE_LDIND_U1:
	case CEE_STIND_I1:
	case CEE_LDELEM_I1:
	case CEE_LDELEM_U1:
	case CEE_STELEM_I1:
		return &mono_defaults.sbyte_class->byval_arg;

	case CEE_LDIND_I2:
	case CEE_LDIND_U2:
	case CEE_STIND_I2:
	case CEE_LDELEM_I2:
	case CEE_LDELEM_U2:
	case CEE_STELEM_I2:
		return &mono_defaults.int16_class->byval_arg;

	case CEE_LDIND_I4:
	case CEE_LDIND_U4:
	case CEE_STIND_I4:
	case CEE_LDELEM_I4:
	case CEE_LDELEM_U4:
	case CEE_STELEM_I4:
		return &mono_defaults.int32_class->byval_arg;

	case CEE_LDIND_I8:
	case CEE_STIND_I8:
	case CEE_LDELEM_I8:
	case CEE_STELEM_I8:
		return &mono_defaults.int64_class->byval_arg;

	case CEE_LDIND_R4:
	case CEE_STIND_R4:
	case CEE_LDELEM_R4:
	case CEE_STELEM_R4:
		return &mono_defaults.single_class->byval_arg;

	case CEE_LDIND_R8:
	case CEE_STIND_R8:
	case CEE_LDELEM_R8:
	case CEE_STELEM_R8:
		return &mono_defaults.double_class->byval_arg;

	case CEE_LDIND_I:
	case CEE_STIND_I:
	case CEE_LDELEM_I:
	case CEE_STELEM_I:
		return &mono_defaults.int_class->byval_arg;

	case CEE_LDIND_REF:
	case CEE_STIND_REF:
	case CEE_LDELEM_REF:
	case CEE_STELEM_REF:
		return &mono_defaults.object_class->byval_arg;

	default:
		g_error ("unknown opcode %02x in mono_type_from_opcode ", opcode);
		return NULL;
	}
}