void pb_controller::usage(const char * argv0) {
	auto msg =
	    strprintf::fmt(_("%s %s\nusage %s [-C <file>] [-q <file>] [-h]\n"),
	    "podbeuter",
	    PROGRAM_VERSION,
	    argv0);
	std::cout << msg;

	struct arg {
		const char name;
		const std::string longname;
		const std::string params;
		const std::string desc;
	};

	static const std::vector<arg> args = {
		{ 'C', "config-file" , _s("<configfile>"), _s("read configuration from <configfile>") }                      ,
		{ 'q', "queue-file"  , _s("<queuefile>") , _s("use <queuefile> as queue file") }                             ,
		{ 'a', "autodownload", ""                , _s("start download on startup") }                                 ,
		{ 'l', "log-level"   , _s("<loglevel>")  , _s("write a log with a certain loglevel (valid values: 1 to 6)") },
		{ 'd', "log-file"    , _s("<logfile>")   , _s("use <logfile> as output log file") }                          ,
		{ 'h', "help"        , ""                , _s("this help") }
	};

	for (const auto & a : args) {
		std::string longcolumn("-");
		longcolumn += a.name;
		longcolumn += ", --" + a.longname;
		longcolumn += a.params.size() > 0 ? "=" + a.params : "";
		std::cout << "\t" << longcolumn;
		for (unsigned int j = 0; j < utils::gentabs(longcolumn); j++) {
			std::cout << "\t";
		}
		std::cout << a.desc << std::endl;
	}

	::exit(EXIT_FAILURE);
}