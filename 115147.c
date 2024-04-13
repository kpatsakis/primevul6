static inline void HandleThreadId(ThreadVars *tv, Packet *p, StreamTcpThread *stt)
{
    const int idx = (!(PKT_IS_TOSERVER(p)));

    /* assign the thread id to the flow */
    if (unlikely(p->flow->thread_id[idx] == 0)) {
        p->flow->thread_id[idx] = (FlowThreadId)tv->id;
    } else if (unlikely((FlowThreadId)tv->id != p->flow->thread_id[idx])) {
        SCLogDebug("wrong thread: flow has %u, we are %d", p->flow->thread_id[idx], tv->id);
        if (p->pkt_src == PKT_SRC_WIRE) {
            StatsIncr(tv, stt->counter_tcp_wrong_thread);
            if ((p->flow->flags & FLOW_WRONG_THREAD) == 0) {
                p->flow->flags |= FLOW_WRONG_THREAD;
                StreamTcpSetEvent(p, STREAM_WRONG_THREAD);
            }
        }
    }
}