public: bool operator() (const ThreadHandoff &a,
                         const ThreadHandoff &b) const {
    return a.segment_size < b.segment_size;
}