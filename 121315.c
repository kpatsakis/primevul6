void pb_controller::reload_queue(bool remove_unplayed) {
	if (ql) {
		ql->reload(downloads_, remove_unplayed);
	}
}