__u32 sctp_association_get_next_tsn(struct sctp_association *asoc)
{
	/* From Section 1.6 Serial Number Arithmetic:
	 * Transmission Sequence Numbers wrap around when they reach
	 * 2**32 - 1.  That is, the next TSN a DATA chunk MUST use
	 * after transmitting TSN = 2*32 - 1 is TSN = 0.
	 */
	__u32 retval = asoc->next_tsn;
	asoc->next_tsn++;
	asoc->unack_data++;

	return retval;
}