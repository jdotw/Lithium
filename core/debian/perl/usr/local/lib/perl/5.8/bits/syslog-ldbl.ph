require '_h2ph_pre.ph';

no warnings 'redefine';

unless(defined(&_SYS_SYSLOG_H)) {
    die("Never include <bits/syslog-ldbl.h> directly; use <sys/syslog.h> instead.");
}
if(defined(&__USE_BSD)) {
}
if((defined(&__USE_FORTIFY_LEVEL) ? &__USE_FORTIFY_LEVEL : undef) > 0 && defined (defined(&__extern_always_inline) ? &__extern_always_inline : undef)) {
    if(defined(&__USE_BSD)) {
    }
}
1;
