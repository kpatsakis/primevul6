static inline void wrmsrl(unsigned msr, u64 val)
{
	wrmsr(msr, (u32)val, (u32)(val>>32));
}