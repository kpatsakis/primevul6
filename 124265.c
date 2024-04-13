static int map_ldb_error(TALLOC_CTX *mem_ctx, int ldb_err,
	const char *add_err_string, const char **errstring)
{
	WERROR err;

	/* Certain LDB modules need to return very special WERROR codes. Proof
	 * for them here and if they exist skip the rest of the mapping. */
	if (add_err_string != NULL) {
		char *endptr;
		strtol(add_err_string, &endptr, 16);
		if (endptr != add_err_string) {
			*errstring = add_err_string;
			return ldb_err;
		}
	}

	/* Otherwise we calculate here a generic, but appropriate WERROR. */

	switch (ldb_err) {
	case LDB_SUCCESS:
		err = WERR_OK;
	break;
	case LDB_ERR_OPERATIONS_ERROR:
		err = WERR_DS_OPERATIONS_ERROR;
	break;
	case LDB_ERR_PROTOCOL_ERROR:
		err = WERR_DS_PROTOCOL_ERROR;
	break;
	case LDB_ERR_TIME_LIMIT_EXCEEDED:
		err = WERR_DS_TIMELIMIT_EXCEEDED;
	break;
	case LDB_ERR_SIZE_LIMIT_EXCEEDED:
		err = WERR_DS_SIZELIMIT_EXCEEDED;
	break;
	case LDB_ERR_COMPARE_FALSE:
		err = WERR_DS_COMPARE_FALSE;
	break;
	case LDB_ERR_COMPARE_TRUE:
		err = WERR_DS_COMPARE_TRUE;
	break;
	case LDB_ERR_AUTH_METHOD_NOT_SUPPORTED:
		err = WERR_DS_AUTH_METHOD_NOT_SUPPORTED;
	break;
	case LDB_ERR_STRONG_AUTH_REQUIRED:
		err = WERR_DS_STRONG_AUTH_REQUIRED;
	break;
	case LDB_ERR_REFERRAL:
		err = WERR_DS_REFERRAL;
	break;
	case LDB_ERR_ADMIN_LIMIT_EXCEEDED:
		err = WERR_DS_ADMIN_LIMIT_EXCEEDED;
	break;
	case LDB_ERR_UNSUPPORTED_CRITICAL_EXTENSION:
		err = WERR_DS_UNAVAILABLE_CRIT_EXTENSION;
	break;
	case LDB_ERR_CONFIDENTIALITY_REQUIRED:
		err = WERR_DS_CONFIDENTIALITY_REQUIRED;
	break;
	case LDB_ERR_SASL_BIND_IN_PROGRESS:
		err = WERR_DS_BUSY;
	break;
	case LDB_ERR_NO_SUCH_ATTRIBUTE:
		err = WERR_DS_NO_ATTRIBUTE_OR_VALUE;
	break;
	case LDB_ERR_UNDEFINED_ATTRIBUTE_TYPE:
		err = WERR_DS_ATTRIBUTE_TYPE_UNDEFINED;
	break;
	case LDB_ERR_INAPPROPRIATE_MATCHING:
		err = WERR_DS_INAPPROPRIATE_MATCHING;
	break;
	case LDB_ERR_CONSTRAINT_VIOLATION:
		err = WERR_DS_CONSTRAINT_VIOLATION;
	break;
	case LDB_ERR_ATTRIBUTE_OR_VALUE_EXISTS:
		err = WERR_DS_ATTRIBUTE_OR_VALUE_EXISTS;
	break;
	case LDB_ERR_INVALID_ATTRIBUTE_SYNTAX:
		err = WERR_DS_INVALID_ATTRIBUTE_SYNTAX;
	break;
	case LDB_ERR_NO_SUCH_OBJECT:
		err = WERR_DS_NO_SUCH_OBJECT;
	break;
	case LDB_ERR_ALIAS_PROBLEM:
		err = WERR_DS_ALIAS_PROBLEM;
	break;
	case LDB_ERR_INVALID_DN_SYNTAX:
		err = WERR_DS_INVALID_DN_SYNTAX;
	break;
	case LDB_ERR_ALIAS_DEREFERENCING_PROBLEM:
		err = WERR_DS_ALIAS_DEREF_PROBLEM;
	break;
	case LDB_ERR_INAPPROPRIATE_AUTHENTICATION:
		err = WERR_DS_INAPPROPRIATE_AUTH;
	break;
	case LDB_ERR_INVALID_CREDENTIALS:
		err = WERR_ACCESS_DENIED;
	break;
	case LDB_ERR_INSUFFICIENT_ACCESS_RIGHTS:
		err = WERR_DS_INSUFF_ACCESS_RIGHTS;
	break;
	case LDB_ERR_BUSY:
		err = WERR_DS_BUSY;
	break;
	case LDB_ERR_UNAVAILABLE:
		err = WERR_DS_UNAVAILABLE;
	break;
	case LDB_ERR_UNWILLING_TO_PERFORM:
		err = WERR_DS_UNWILLING_TO_PERFORM;
	break;
	case LDB_ERR_LOOP_DETECT:
		err = WERR_DS_LOOP_DETECT;
	break;
	case LDB_ERR_NAMING_VIOLATION:
		err = WERR_DS_NAMING_VIOLATION;
	break;
	case LDB_ERR_OBJECT_CLASS_VIOLATION:
		err = WERR_DS_OBJ_CLASS_VIOLATION;
	break;
	case LDB_ERR_NOT_ALLOWED_ON_NON_LEAF:
		err = WERR_DS_CANT_ON_NON_LEAF;
	break;
	case LDB_ERR_NOT_ALLOWED_ON_RDN:
		err = WERR_DS_CANT_ON_RDN;
	break;
	case LDB_ERR_ENTRY_ALREADY_EXISTS:
		err = WERR_DS_OBJ_STRING_NAME_EXISTS;
	break;
	case LDB_ERR_OBJECT_CLASS_MODS_PROHIBITED:
		err = WERR_DS_CANT_MOD_OBJ_CLASS;
	break;
	case LDB_ERR_AFFECTS_MULTIPLE_DSAS:
		err = WERR_DS_AFFECTS_MULTIPLE_DSAS;
	break;
	default:
		err = WERR_DS_GENERIC_ERROR;
	break;
	}

	*errstring = talloc_asprintf(mem_ctx, "%08X: %s", W_ERROR_V(err),
		add_err_string != NULL ? add_err_string : ldb_strerror(ldb_err));

	/* result is 1:1 for now */
	return ldb_err;
}