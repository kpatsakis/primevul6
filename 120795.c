idprime_set_security_env(struct sc_card *card,
	const struct sc_security_env *env, int se_num)
{
	int r;
	struct sc_security_env new_env;

	if (card == NULL || env == NULL) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	/* The card requires algorithm reference here */
	new_env = *env;
	new_env.flags |= SC_SEC_ENV_ALG_REF_PRESENT;
	/* SHA-1 mechanisms are not allowed in the card I have available */
	switch (env->operation) {
	case SC_SEC_OPERATION_DECIPHER:
		if (env->algorithm_flags & SC_ALGORITHM_RSA_PAD_OAEP) {
			if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA1) {
				new_env.algorithm_ref = 0x1D;
			} else if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA256) {
				new_env.algorithm_ref = 0x4D;
			} else if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA384) {
				new_env.algorithm_ref = 0x5D;
			} else if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA512) {
				new_env.algorithm_ref = 0x6D;
			}
		} else { /* RSA-PKCS without hashing */
			new_env.algorithm_ref = 0x1A;
		}
		break;
	case SC_SEC_OPERATION_SIGN:
		if (env->algorithm_flags & SC_ALGORITHM_RSA_PAD_PSS) {
			if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA256) {
				new_env.algorithm_ref = 0x45;
			} else if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA384) {
				new_env.algorithm_ref = 0x55;
			} else if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA512) {
				new_env.algorithm_ref = 0x65;
			}
		} else { /* RSA-PKCS */
			if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA256) {
				new_env.algorithm_ref = 0x42;
			} else if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA384) {
				new_env.algorithm_ref = 0x52;
			} else if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA512) {
				new_env.algorithm_ref = 0x62;
			} else { /* RSA-PKCS without hashing */
				new_env.algorithm_ref = 0x02;
			}
		}
		break;
	default:
		return SC_ERROR_INVALID_ARGUMENTS;
	}
	r = iso_ops->set_security_env(card,
		(const struct sc_security_env *) &new_env, se_num);

	LOG_FUNC_RETURN(card->ctx, r);
}