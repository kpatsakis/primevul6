static int putreg(struct task_struct *child,
	unsigned long regno, unsigned long value)
{
	unsigned long tmp; 
	
	/* Some code in the 64bit emulation may not be 64bit clean.
	   Don't take any chances. */
	if (test_tsk_thread_flag(child, TIF_IA32))
		value &= 0xffffffff;
	switch (regno) {
		case offsetof(struct user_regs_struct,fs):
			if (value && (value & 3) != 3)
				return -EIO;
			child->thread.fsindex = value & 0xffff; 
			return 0;
		case offsetof(struct user_regs_struct,gs):
			if (value && (value & 3) != 3)
				return -EIO;
			child->thread.gsindex = value & 0xffff;
			return 0;
		case offsetof(struct user_regs_struct,ds):
			if (value && (value & 3) != 3)
				return -EIO;
			child->thread.ds = value & 0xffff;
			return 0;
		case offsetof(struct user_regs_struct,es): 
			if (value && (value & 3) != 3)
				return -EIO;
			child->thread.es = value & 0xffff;
			return 0;
		case offsetof(struct user_regs_struct,ss):
			if ((value & 3) != 3)
				return -EIO;
			value &= 0xffff;
			return 0;
		case offsetof(struct user_regs_struct,fs_base):
			if (value >= TASK_SIZE_OF(child))
				return -EIO;
			child->thread.fs = value;
			return 0;
		case offsetof(struct user_regs_struct,gs_base):
			if (value >= TASK_SIZE_OF(child))
				return -EIO;
			child->thread.gs = value;
			return 0;
		case offsetof(struct user_regs_struct, eflags):
			value &= FLAG_MASK;
			tmp = get_stack_long(child, EFL_OFFSET); 
			tmp &= ~FLAG_MASK; 
			value |= tmp;
			break;
		case offsetof(struct user_regs_struct,cs): 
			if ((value & 3) != 3)
				return -EIO;
			value &= 0xffff;
			break;
	}
	put_stack_long(child, regno - sizeof(struct pt_regs), value);
	return 0;
}