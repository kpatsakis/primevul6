static void qrtr_reset_ports(void)
{
	struct qrtr_sock *ipc;
	unsigned long index;

	rcu_read_lock();
	xa_for_each_start(&qrtr_ports, index, ipc, 1) {
		sock_hold(&ipc->sk);
		ipc->sk.sk_err = ENETRESET;
		sk_error_report(&ipc->sk);
		sock_put(&ipc->sk);
	}
	rcu_read_unlock();
}