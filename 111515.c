void tb_flush(CPUState *cpu)
{
    unsigned tb_flush_count = cpu->uc->tcg_ctx->tb_ctx.tb_flush_count;
    do_tb_flush(cpu, RUN_ON_CPU_HOST_INT(tb_flush_count));
}