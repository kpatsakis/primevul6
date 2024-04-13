static int smack_socket_bind(struct socket *sock, struct sockaddr *address,
				int addrlen)
{
	if (sock->sk != NULL && sock->sk->sk_family == PF_INET6) {
		if (addrlen < SIN6_LEN_RFC2133 ||
		    address->sa_family != AF_INET6)
			return -EINVAL;
		smk_ipv6_port_label(sock, address);
	}
	return 0;
}