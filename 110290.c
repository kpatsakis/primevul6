static int compat_get_timex(struct timex *txc, struct compat_timex __user *utp)
{
	memset(txc, 0, sizeof(struct timex));

	if (!access_ok(VERIFY_READ, utp, sizeof(struct compat_timex)) ||
			__get_user(txc->modes, &utp->modes) ||
			__get_user(txc->offset, &utp->offset) ||
			__get_user(txc->freq, &utp->freq) ||
			__get_user(txc->maxerror, &utp->maxerror) ||
			__get_user(txc->esterror, &utp->esterror) ||
			__get_user(txc->status, &utp->status) ||
			__get_user(txc->constant, &utp->constant) ||
			__get_user(txc->precision, &utp->precision) ||
			__get_user(txc->tolerance, &utp->tolerance) ||
			__get_user(txc->time.tv_sec, &utp->time.tv_sec) ||
			__get_user(txc->time.tv_usec, &utp->time.tv_usec) ||
			__get_user(txc->tick, &utp->tick) ||
			__get_user(txc->ppsfreq, &utp->ppsfreq) ||
			__get_user(txc->jitter, &utp->jitter) ||
			__get_user(txc->shift, &utp->shift) ||
			__get_user(txc->stabil, &utp->stabil) ||
			__get_user(txc->jitcnt, &utp->jitcnt) ||
			__get_user(txc->calcnt, &utp->calcnt) ||
			__get_user(txc->errcnt, &utp->errcnt) ||
			__get_user(txc->stbcnt, &utp->stbcnt))
		return -EFAULT;

	return 0;
}