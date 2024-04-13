static void handle_signal_self(int flag)
{
	if ((flag & (RADIUS_SIGNAL_SELF_EXIT | RADIUS_SIGNAL_SELF_TERM)) != 0) {
		if ((flag & RADIUS_SIGNAL_SELF_EXIT) != 0) {
			fr_event_loop_exit(el, 1);
		} else {
			fr_event_loop_exit(el, 2);
		}

		return;
	} /* else exit/term flags weren't set */

	/*
	 *	Tell the even loop to stop processing.
	 */
	if ((flag & RADIUS_SIGNAL_SELF_HUP) != 0) {
		time_t when;
		static time_t last_hup = 0;

		when = time(NULL);
		if ((int) (when - last_hup) < 5) {
			radlog(L_INFO, "Ignoring HUP (less than 5s since last one)");
			return;
		}

		radlog(L_INFO, "Received HUP signal.");

		last_hup = when;

		fr_event_loop_exit(el, 0x80);
	}

#ifdef WITH_DETAIL
	if ((flag & RADIUS_SIGNAL_SELF_DETAIL) != 0) {
		rad_listen_t *this;
		
		/*
		 *	FIXME: O(N) loops suck.
		 */
		for (this = mainconfig.listen;
		     this != NULL;
		     this = this->next) {
			if (this->type != RAD_LISTEN_DETAIL) continue;

			/*
			 *	This one didn't send the signal, skip
			 *	it.
			 */
			if (!this->decode(this, NULL)) continue;

			/*
			 *	Go service the interrupt.
			 */
			event_poll_detail(this);
		}
	}
#endif

	if ((flag & RADIUS_SIGNAL_SELF_NEW_FD) != 0) {
		rad_listen_t *this;
		
		for (this = mainconfig.listen;
		     this != NULL;
		     this = this->next) {
			event_new_fd(this);
		}
	}
}