static int selinux_netlink_send(struct sock *sk, struct sk_buff *skb)
{
	int rc = 0;
	unsigned int msg_len;
	unsigned int data_len = skb->len;
	unsigned char *data = skb->data;
	struct nlmsghdr *nlh;
	struct sk_security_struct *sksec = sk->sk_security;
	u16 sclass = sksec->sclass;
	u32 perm;

	while (data_len >= nlmsg_total_size(0)) {
		nlh = (struct nlmsghdr *)data;

		/* NOTE: the nlmsg_len field isn't reliably set by some netlink
		 *       users which means we can't reject skb's with bogus
		 *       length fields; our solution is to follow what
		 *       netlink_rcv_skb() does and simply skip processing at
		 *       messages with length fields that are clearly junk
		 */
		if (nlh->nlmsg_len < NLMSG_HDRLEN || nlh->nlmsg_len > data_len)
			return 0;

		rc = selinux_nlmsg_lookup(sclass, nlh->nlmsg_type, &perm);
		if (rc == 0) {
			rc = sock_has_perm(sk, perm);
			if (rc)
				return rc;
		} else if (rc == -EINVAL) {
			/* -EINVAL is a missing msg/perm mapping */
			pr_warn_ratelimited("SELinux: unrecognized netlink"
				" message: protocol=%hu nlmsg_type=%hu sclass=%s"
				" pid=%d comm=%s\n",
				sk->sk_protocol, nlh->nlmsg_type,
				secclass_map[sclass - 1].name,
				task_pid_nr(current), current->comm);
			if (enforcing_enabled(&selinux_state) &&
			    !security_get_allow_unknown(&selinux_state))
				return rc;
			rc = 0;
		} else if (rc == -ENOENT) {
			/* -ENOENT is a missing socket/class mapping, ignore */
			rc = 0;
		} else {
			return rc;
		}

		/* move to the next message after applying netlink padding */
		msg_len = NLMSG_ALIGN(nlh->nlmsg_len);
		if (msg_len >= data_len)
			return 0;
		data_len -= msg_len;
		data += msg_len;
	}

	return rc;
}