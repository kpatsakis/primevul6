void pb_controller::decrease_parallel_downloads() {
	if (max_dls > 1)
		--max_dls;
}