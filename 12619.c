static void selinux_sk_getsecid(struct sock *sk, u32 *secid)
{
	if (!sk)
		*secid = SECINITSID_ANY_SOCKET;
	else {
		struct sk_security_struct *sksec = sk->sk_security;

		*secid = sksec->sid;
	}
}