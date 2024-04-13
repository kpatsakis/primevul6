MYSOFA_EXPORT void mysofa_getversion(int *major, int *minor, int *patch) {
	*major = CPACK_PACKAGE_VERSION_MAJOR;
	*minor = CPACK_PACKAGE_VERSION_MINOR;
	*patch = CPACK_PACKAGE_VERSION_PATCH;
}