void event_new_fd(rad_listen_t *this)
{
	char buffer[1024];

	if (this->status == RAD_LISTEN_STATUS_KNOWN) return;
	
	this->print(this, buffer, sizeof(buffer));
	
	if (this->status == RAD_LISTEN_STATUS_INIT) {
		if (just_started) {
			DEBUG("Listening on %s", buffer);
		} else {
			radlog(L_INFO, " ... adding new socket %s", buffer);
		}
		if (!fr_event_fd_insert(el, 0, this->fd,
					event_socket_handler, this)) {
			radlog(L_ERR, "Failed remembering handle for proxy socket!");
			exit(1);
		}
		
		this->status = RAD_LISTEN_STATUS_KNOWN;
		return;
	}
	
	if (this->status == RAD_LISTEN_STATUS_CLOSED) {
		radlog(L_INFO, " ... closing socket %s", buffer);
		
		fr_event_fd_delete(el, 0, this->fd);
		this->status = RAD_LISTEN_STATUS_FINISH;
		
		/*
		 *	Close the fd AFTER fixing up the requests and
		 *	listeners, so that they don't send/recv on the
		 *	wrong socket (if someone manages to open
		 *	another one).
		 */
		close(this->fd);
		this->fd = -1;
	}
}