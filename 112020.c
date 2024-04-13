static int qrtr_port_assign(struct qrtr_sock *ipc, int *port)
{
	int rc;

	if (!*port) {
		rc = xa_alloc(&qrtr_ports, port, ipc, QRTR_EPH_PORT_RANGE,
				GFP_KERNEL);
	} else if (*port < QRTR_MIN_EPH_SOCKET && !capable(CAP_NET_ADMIN)) {
		rc = -EACCES;
	} else if (*port == QRTR_PORT_CTRL) {
		rc = xa_insert(&qrtr_ports, 0, ipc, GFP_KERNEL);
	} else {
		rc = xa_insert(&qrtr_ports, *port, ipc, GFP_KERNEL);
	}

	if (rc == -EBUSY)
		return -EADDRINUSE;
	else if (rc < 0)
		return rc;

	sock_hold(&ipc->sk);

	return 0;
}