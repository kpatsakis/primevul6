int tipc_sk_dump(struct sock *sk, u16 dqueues, char *buf)
{
	int i = 0;
	size_t sz = (dqueues) ? SK_LMAX : SK_LMIN;
	u32 conn_type, conn_instance;
	struct tipc_sock *tsk;
	struct publication *p;
	bool tsk_connected;

	if (!sk) {
		i += scnprintf(buf, sz, "sk data: (null)\n");
		return i;
	}

	tsk = tipc_sk(sk);
	tsk_connected = !tipc_sk_type_connectionless(sk);

	i += scnprintf(buf, sz, "sk data: %u", sk->sk_type);
	i += scnprintf(buf + i, sz - i, " %d", sk->sk_state);
	i += scnprintf(buf + i, sz - i, " %x", tsk_own_node(tsk));
	i += scnprintf(buf + i, sz - i, " %u", tsk->portid);
	i += scnprintf(buf + i, sz - i, " | %u", tsk_connected);
	if (tsk_connected) {
		i += scnprintf(buf + i, sz - i, " %x", tsk_peer_node(tsk));
		i += scnprintf(buf + i, sz - i, " %u", tsk_peer_port(tsk));
		conn_type = msg_nametype(&tsk->phdr);
		conn_instance = msg_nameinst(&tsk->phdr);
		i += scnprintf(buf + i, sz - i, " %u", conn_type);
		i += scnprintf(buf + i, sz - i, " %u", conn_instance);
	}
	i += scnprintf(buf + i, sz - i, " | %u", tsk->published);
	if (tsk->published) {
		p = list_first_entry_or_null(&tsk->publications,
					     struct publication, binding_sock);
		i += scnprintf(buf + i, sz - i, " %u", (p) ? p->sr.type : 0);
		i += scnprintf(buf + i, sz - i, " %u", (p) ? p->sr.lower : 0);
		i += scnprintf(buf + i, sz - i, " %u", (p) ? p->sr.upper : 0);
	}
	i += scnprintf(buf + i, sz - i, " | %u", tsk->snd_win);
	i += scnprintf(buf + i, sz - i, " %u", tsk->rcv_win);
	i += scnprintf(buf + i, sz - i, " %u", tsk->max_pkt);
	i += scnprintf(buf + i, sz - i, " %x", tsk->peer_caps);
	i += scnprintf(buf + i, sz - i, " %u", tsk->cong_link_cnt);
	i += scnprintf(buf + i, sz - i, " %u", tsk->snt_unacked);
	i += scnprintf(buf + i, sz - i, " %u", tsk->rcv_unacked);
	i += scnprintf(buf + i, sz - i, " %u", atomic_read(&tsk->dupl_rcvcnt));
	i += scnprintf(buf + i, sz - i, " %u", sk->sk_shutdown);
	i += scnprintf(buf + i, sz - i, " | %d", sk_wmem_alloc_get(sk));
	i += scnprintf(buf + i, sz - i, " %d", sk->sk_sndbuf);
	i += scnprintf(buf + i, sz - i, " | %d", sk_rmem_alloc_get(sk));
	i += scnprintf(buf + i, sz - i, " %d", sk->sk_rcvbuf);
	i += scnprintf(buf + i, sz - i, " | %d\n", READ_ONCE(sk->sk_backlog.len));

	if (dqueues & TIPC_DUMP_SK_SNDQ) {
		i += scnprintf(buf + i, sz - i, "sk_write_queue: ");
		i += tipc_list_dump(&sk->sk_write_queue, false, buf + i);
	}

	if (dqueues & TIPC_DUMP_SK_RCVQ) {
		i += scnprintf(buf + i, sz - i, "sk_receive_queue: ");
		i += tipc_list_dump(&sk->sk_receive_queue, false, buf + i);
	}

	if (dqueues & TIPC_DUMP_SK_BKLGQ) {
		i += scnprintf(buf + i, sz - i, "sk_backlog:\n  head ");
		i += tipc_skb_dump(sk->sk_backlog.head, false, buf + i);
		if (sk->sk_backlog.tail != sk->sk_backlog.head) {
			i += scnprintf(buf + i, sz - i, "  tail ");
			i += tipc_skb_dump(sk->sk_backlog.tail, false,
					   buf + i);
		}
	}

	return i;
}