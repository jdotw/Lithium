require '_h2ph_pre.ph';

no warnings 'redefine';

unless(defined(&_ASM_X86_SOCKIOS_H)) {
    eval 'sub _ASM_X86_SOCKIOS_H () {1;}' unless defined(&_ASM_X86_SOCKIOS_H);
    eval 'sub FIOSETOWN () {0x8901;}' unless defined(&FIOSETOWN);
    eval 'sub SIOCSPGRP () {0x8902;}' unless defined(&SIOCSPGRP);
    eval 'sub FIOGETOWN () {0x8903;}' unless defined(&FIOGETOWN);
    eval 'sub SIOCGPGRP () {0x8904;}' unless defined(&SIOCGPGRP);
    eval 'sub SIOCATMARK () {0x8905;}' unless defined(&SIOCATMARK);
    eval 'sub SIOCGSTAMP () {0x8906;}' unless defined(&SIOCGSTAMP);
    eval 'sub SIOCGSTAMPNS () {0x8907;}' unless defined(&SIOCGSTAMPNS);
}
1;
