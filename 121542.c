bool rst_cnt_ok(int scan, unsigned int num_rst_markers_this_scan) {
    if (rstp.empty()) {
        return false;
    }
    if (!rst_cnt_set) {
        return true;
    }
    return rst_cnt.size() > (size_t)scan - 1 && num_rst_markers_this_scan < rst_cnt.at(scan - 1);
}