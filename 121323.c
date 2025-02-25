void pb_controller::play_file(const std::string& file) {
	std::string cmdline;
	std::string player = cfg->get_configvalue("player");
	if (player == "")
		return;
	cmdline.append(player);
	cmdline.append(" '");
	cmdline.append(utils::replace_all(file,"'", "%27"));
	cmdline.append("'");
	stfl::reset();
	utils::run_interactively(cmdline, "pb_controller::play_file");
}