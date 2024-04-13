auth_sig_compatible(uint8_t type)
{
	switch (type) {
	case IKEV2_AUTH_RSA_SIG:
	case IKEV2_AUTH_ECDSA_256:
	case IKEV2_AUTH_ECDSA_384:
	case IKEV2_AUTH_ECDSA_521:
	case IKEV2_AUTH_SIG_ANY:
		return (1);
	}
	return (0);
}