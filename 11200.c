static int tipc_set_sk_state(struct sock *sk, int state)
{
	int oldsk_state = sk->sk_state;
	int res = -EINVAL;

	switch (state) {
	case TIPC_OPEN:
		res = 0;
		break;
	case TIPC_LISTEN:
	case TIPC_CONNECTING:
		if (oldsk_state == TIPC_OPEN)
			res = 0;
		break;
	case TIPC_ESTABLISHED:
		if (oldsk_state == TIPC_CONNECTING ||
		    oldsk_state == TIPC_OPEN)
			res = 0;
		break;
	case TIPC_DISCONNECTING:
		if (oldsk_state == TIPC_CONNECTING ||
		    oldsk_state == TIPC_ESTABLISHED)
			res = 0;
		break;
	}

	if (!res)
		sk->sk_state = state;

	return res;
}