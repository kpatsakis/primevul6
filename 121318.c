void pb_controller::start_downloads() {
	int dl2start = get_maxdownloads() - downloads_in_progress();
	for (auto& download : downloads_) {
		if (dl2start == 0) break;

		if (download.status() == dlstatus::QUEUED) {
			std::thread t {poddlthread(&download, cfg)};
			--dl2start;
			t.detach();
		}
	}
}