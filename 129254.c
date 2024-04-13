errno_to_str(
	int	err,
	char *	buf,
	size_t	bufsiz
	)
{
# if defined(STRERROR_R_CHAR_P) || !HAVE_DECL_STRERROR_R
	char *	pstatic;

	buf[0] = '\0';
#  ifdef STRERROR_R_CHAR_P
	pstatic = strerror_r(err, buf, bufsiz);
#  else
	pstatic = strerror(err);
#  endif
	if (NULL == pstatic && '\0' == buf[0])
		snprintf(buf, bufsiz, "%s(%d): errno %d",
#  ifdef STRERROR_R_CHAR_P
			 "strerror_r",
#  else
			 "strerror",
#  endif
			 err, errno);
	/* protect against believing an int return is a pointer */
	else if (pstatic != buf && pstatic > (char *)bufsiz)
		strlcpy(buf, pstatic, bufsiz);
# else
	int	rc;

	rc = strerror_r(err, buf, bufsiz);
	if (rc < 0)
		snprintf(buf, bufsiz, "strerror_r(%d): errno %d",
			 err, errno);
# endif
}