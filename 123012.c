ngx_http_time(u_char *buf, time_t t)
{
    ngx_tm_t  tm;

    ngx_gmtime(t, &tm);

    return ngx_sprintf(buf, "%s, %02d %s %4d %02d:%02d:%02d GMT",
                       week[tm.ngx_tm_wday],
                       tm.ngx_tm_mday,
                       months[tm.ngx_tm_mon - 1],
                       tm.ngx_tm_year,
                       tm.ngx_tm_hour,
                       tm.ngx_tm_min,
                       tm.ngx_tm_sec);
}