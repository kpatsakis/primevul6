R_API void r_bin_java_verification_info_free(void /*RBinJavaVerificationObj*/ *o) {
	RBinJavaVerificationObj *obj = o;
	// eprintf ("Freeing verification object\n");
	if (obj) {
		free (obj->name);
		free (obj);
	}
}