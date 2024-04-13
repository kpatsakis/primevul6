void pb_controller::run(int argc, char * argv[]) {
	int c;
	bool automatic_dl = false;

	::signal(SIGINT, ctrl_c_action);

	static const char getopt_str[] = "C:q:d:l:havV";
	static const struct option longopts[] = {
		{"config-file"     , required_argument, 0, 'C'},
		{"queue-file"      , required_argument, 0, 'q'},
		{"log-file"        , required_argument, 0, 'd'},
		{"log-level"       , required_argument, 0, 'l'},
		{"help"            , no_argument      , 0, 'h'},
		{"autodownload"    , no_argument      , 0, 'a'},
		{"version"         , no_argument      , 0, 'v'},
		{0                 , 0                , 0,  0 }
	};

	while ((c = ::getopt_long(argc, argv, getopt_str, longopts, nullptr)) != -1) {
		switch (c) {
		case ':':
		case '?':
			usage(argv[0]);
			break;
		case 'C':
			config_file = optarg;
			break;
		case 'q':
			queue_file = optarg;
			break;
		case 'a':
			automatic_dl = true;
			break;
		case 'd':
			logger::getInstance().set_logfile(optarg);
			break;
		case 'l': {
			level l = static_cast<level>(atoi(optarg));
			if (l > level::NONE && l <= level::DEBUG) {
				logger::getInstance().set_loglevel(l);
			} else {
				std::cerr << strprintf::fmt(_("%s: %d: invalid loglevel value"), argv[0], l) << std::endl;
				::std::exit(EXIT_FAILURE);
			}
			}
			break;
		case 'h':
			usage(argv[0]);
			break;
		default:
			std::cout << strprintf::fmt(_("%s: unknown option - %c"), argv[0], static_cast<char>(c)) << std::endl;
			usage(argv[0]);
			break;
		}
	};

	std::cout << strprintf::fmt(_("Starting %s %s..."), "podbeuter", PROGRAM_VERSION) << std::endl;

	pid_t pid;
	if (!utils::try_fs_lock(lock_file, pid)) {
		std::cout << strprintf::fmt(_("Error: an instance of %s is already running (PID: %u)"), "podbeuter", pid) << std::endl;
		return;
	}

	std::cout << _("Loading configuration...");
	std::cout.flush();

	configparser cfgparser;
	cfg = new configcontainer();
	cfg->register_commands(cfgparser);
	colormanager * colorman = new colormanager();
	colorman->register_commands(cfgparser);

	keymap keys(KM_PODBEUTER);
	cfgparser.register_handler("bind-key", &keys);
	cfgparser.register_handler("unbind-key", &keys);

	null_config_action_handler null_cah;
	cfgparser.register_handler("macro", &null_cah);
	cfgparser.register_handler("ignore-article", &null_cah);
	cfgparser.register_handler("always-download", &null_cah);
	cfgparser.register_handler("define-filter", &null_cah);
	cfgparser.register_handler("highlight", &null_cah);
	cfgparser.register_handler("highlight-article", &null_cah);
	cfgparser.register_handler("reset-unread-on-update", &null_cah);

	try {
		cfgparser.parse("/etc/newsbeuter/config");
		cfgparser.parse(config_file);
	} catch (const configexception& ex) {
		std::cout << ex.what() << std::endl;
		delete colorman;
		return;
	}

	if (colorman->colors_loaded())
		colorman->set_pb_colors(v);
	delete colorman;

	max_dls = cfg->get_configvalue_as_int("max-downloads");

	std::cout << _("done.") << std::endl;

	ql = new queueloader(queue_file, this);
	ql->reload(downloads_);

	v->set_keymap(&keys);

	v->run(automatic_dl);

	stfl::reset();

	std::cout <<  _("Cleaning up queue...");
	std::cout.flush();

	ql->reload(downloads_);
	delete ql;

	std::cout << _("done.") << std::endl;

	utils::remove_fs_lock(lock_file);
}