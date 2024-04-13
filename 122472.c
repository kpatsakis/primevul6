int rad_packet_send(struct rad_packet_t *pack, int fd, struct sockaddr_in *addr)
{
	int n;

	clock_gettime(CLOCK_MONOTONIC, &pack->tv);

	while (1) {
		if (addr)
			n = sendto(fd, pack->buf, pack->len, 0, addr, sizeof(*addr));
		else
			n = write(fd, pack->buf, pack->len);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			log_ppp_error("radius:write: %s\n", strerror(errno));
			return -1;
		} else if (n != pack->len) {
			log_ppp_error("radius:write: short write %i, excpected %i\n", n, pack->len);
			return -1;
		}
		break;
	}

	return 0;
}