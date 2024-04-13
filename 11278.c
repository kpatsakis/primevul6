bool tipc_sk_filtering(struct sock *sk)
{
	struct tipc_sock *tsk;
	struct publication *p;
	u32 _port, _sktype, _type, _lower, _upper;
	u32 type = 0, lower = 0, upper = 0;

	if (!sk)
		return true;

	tsk = tipc_sk(sk);

	_port = sysctl_tipc_sk_filter[0];
	_sktype = sysctl_tipc_sk_filter[1];
	_type = sysctl_tipc_sk_filter[2];
	_lower = sysctl_tipc_sk_filter[3];
	_upper = sysctl_tipc_sk_filter[4];

	if (!_port && !_sktype && !_type && !_lower && !_upper)
		return true;

	if (_port)
		return (_port == tsk->portid);

	if (_sktype && _sktype != sk->sk_type)
		return false;

	if (tsk->published) {
		p = list_first_entry_or_null(&tsk->publications,
					     struct publication, binding_sock);
		if (p) {
			type = p->sr.type;
			lower = p->sr.lower;
			upper = p->sr.upper;
		}
	}

	if (!tipc_sk_type_connectionless(sk)) {
		type = msg_nametype(&tsk->phdr);
		lower = msg_nameinst(&tsk->phdr);
		upper = lower;
	}

	if ((_type && _type != type) || (_lower && _lower != lower) ||
	    (_upper && _upper != upper))
		return false;

	return true;
}