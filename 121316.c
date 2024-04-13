double pb_controller::get_total_kbps() {
	double result = 0.0;
	for (auto dl : downloads_) {
		if (dl.status() == dlstatus::DOWNLOADING) {
			result += dl.kbps();
		}
	}
	return result;
}