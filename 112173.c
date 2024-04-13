RandomCancelKey(int32 *cancel_key)
{
	return pg_strong_random(cancel_key, sizeof(int32));
}