runAndPrintExceptions(const boost::function<void ()> &func, bool toAbort) {
	try {
		func();
	} catch (const boost::thread_interrupted &) {
		throw;
	} catch (const tracable_exception &e) {
		P_ERROR("Exception: " << e.what() << "\n" << e.backtrace());
		if (toAbort) {
			abort();
		}
	}
}