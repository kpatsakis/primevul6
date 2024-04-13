static int tipc_wait_for_rcvmsg(struct socket *sock, long *timeop)
{
	struct sock *sk = sock->sk;
	DEFINE_WAIT_FUNC(wait, woken_wake_function);
	long timeo = *timeop;
	int err = sock_error(sk);

	if (err)
		return err;

	for (;;) {
		if (timeo && skb_queue_empty(&sk->sk_receive_queue)) {
			if (sk->sk_shutdown & RCV_SHUTDOWN) {
				err = -ENOTCONN;
				break;
			}
			add_wait_queue(sk_sleep(sk), &wait);
			release_sock(sk);
			timeo = wait_woken(&wait, TASK_INTERRUPTIBLE, timeo);
			sched_annotate_sleep();
			lock_sock(sk);
			remove_wait_queue(sk_sleep(sk), &wait);
		}
		err = 0;
		if (!skb_queue_empty(&sk->sk_receive_queue))
			break;
		err = -EAGAIN;
		if (!timeo)
			break;
		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			break;

		err = sock_error(sk);
		if (err)
			break;
	}
	*timeop = timeo;
	return err;
}