static void smack_sk_free_security(struct sock *sk)
{
#ifdef SMACK_IPV6_PORT_LABELING
	struct smk_port_label *spp;

	if (sk->sk_family == PF_INET6) {
		rcu_read_lock();
		list_for_each_entry_rcu(spp, &smk_ipv6_port_list, list) {
			if (spp->smk_sock != sk)
				continue;
			spp->smk_can_reuse = 1;
			break;
		}
		rcu_read_unlock();
	}
#endif
	kfree(sk->sk_security);
}