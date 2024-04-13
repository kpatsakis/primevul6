inc_tx_bcast_or_mcast_count(E1000State *s, const unsigned char *arr)
{
    if (!memcmp(arr, bcast, sizeof bcast)) {
        e1000x_inc_reg_if_not_full(s->mac_reg, BPTC);
    } else if (arr[0] & 1) {
        e1000x_inc_reg_if_not_full(s->mac_reg, MPTC);
    }
}