unsigned int pb_controller::downloads_in_progress() {
	unsigned int count = 0;
	for (auto dl : downloads_) {
		if (dl.status() == dlstatus::DOWNLOADING)
			++count;
	}
	return count;
}