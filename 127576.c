getHostName() {
	long hostNameMax = HOST_NAME_MAX;
	if (hostNameMax < 255) {
		// https://bugzilla.redhat.com/show_bug.cgi?id=130733
		hostNameMax = 255;
	}

	string buf(hostNameMax + 1, '\0');
	if (gethostname(&buf[0], hostNameMax + 1) == 0) {
		buf[hostNameMax] = '\0';
		return string(buf.c_str());
	} else {
		int e = errno;
		throw SystemException("Unable to query the system's host name", e);
	}
}