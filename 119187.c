int cgtimer_to_ms(cgtimer_t *cgt)
{
	return (int)(cgt->QuadPart / 10000LL);
}