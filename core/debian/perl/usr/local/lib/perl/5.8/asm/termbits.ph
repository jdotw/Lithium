require '_h2ph_pre.ph';

no warnings 'redefine';

unless(defined(&_ASM_X86_TERMBITS_H)) {
    eval 'sub _ASM_X86_TERMBITS_H () {1;}' unless defined(&_ASM_X86_TERMBITS_H);
    require 'linux/posix_types.ph';
    eval 'sub NCCS () {19;}' unless defined(&NCCS);
    eval 'sub VINTR () {0;}' unless defined(&VINTR);
    eval 'sub VQUIT () {1;}' unless defined(&VQUIT);
    eval 'sub VERASE () {2;}' unless defined(&VERASE);
    eval 'sub VKILL () {3;}' unless defined(&VKILL);
    eval 'sub VEOF () {4;}' unless defined(&VEOF);
    eval 'sub VTIME () {5;}' unless defined(&VTIME);
    eval 'sub VMIN () {6;}' unless defined(&VMIN);
    eval 'sub VSWTC () {7;}' unless defined(&VSWTC);
    eval 'sub VSTART () {8;}' unless defined(&VSTART);
    eval 'sub VSTOP () {9;}' unless defined(&VSTOP);
    eval 'sub VSUSP () {10;}' unless defined(&VSUSP);
    eval 'sub VEOL () {11;}' unless defined(&VEOL);
    eval 'sub VREPRINT () {12;}' unless defined(&VREPRINT);
    eval 'sub VDISCARD () {13;}' unless defined(&VDISCARD);
    eval 'sub VWERASE () {14;}' unless defined(&VWERASE);
    eval 'sub VLNEXT () {15;}' unless defined(&VLNEXT);
    eval 'sub VEOL2 () {16;}' unless defined(&VEOL2);
    eval 'sub IGNBRK () {0000001;}' unless defined(&IGNBRK);
    eval 'sub BRKINT () {0000002;}' unless defined(&BRKINT);
    eval 'sub IGNPAR () {0000004;}' unless defined(&IGNPAR);
    eval 'sub PARMRK () {0000010;}' unless defined(&PARMRK);
    eval 'sub INPCK () {0000020;}' unless defined(&INPCK);
    eval 'sub ISTRIP () {0000040;}' unless defined(&ISTRIP);
    eval 'sub INLCR () {0000100;}' unless defined(&INLCR);
    eval 'sub IGNCR () {0000200;}' unless defined(&IGNCR);
    eval 'sub ICRNL () {0000400;}' unless defined(&ICRNL);
    eval 'sub IUCLC () {0001000;}' unless defined(&IUCLC);
    eval 'sub IXON () {0002000;}' unless defined(&IXON);
    eval 'sub IXANY () {0004000;}' unless defined(&IXANY);
    eval 'sub IXOFF () {0010000;}' unless defined(&IXOFF);
    eval 'sub IMAXBEL () {0020000;}' unless defined(&IMAXBEL);
    eval 'sub IUTF8 () {0040000;}' unless defined(&IUTF8);
    eval 'sub OPOST () {0000001;}' unless defined(&OPOST);
    eval 'sub OLCUC () {0000002;}' unless defined(&OLCUC);
    eval 'sub ONLCR () {0000004;}' unless defined(&ONLCR);
    eval 'sub OCRNL () {0000010;}' unless defined(&OCRNL);
    eval 'sub ONOCR () {0000020;}' unless defined(&ONOCR);
    eval 'sub ONLRET () {0000040;}' unless defined(&ONLRET);
    eval 'sub OFILL () {0000100;}' unless defined(&OFILL);
    eval 'sub OFDEL () {0000200;}' unless defined(&OFDEL);
    eval 'sub NLDLY () {0000400;}' unless defined(&NLDLY);
    eval 'sub NL0 () {0000000;}' unless defined(&NL0);
    eval 'sub NL1 () {0000400;}' unless defined(&NL1);
    eval 'sub CRDLY () {0003000;}' unless defined(&CRDLY);
    eval 'sub CR0 () {0000000;}' unless defined(&CR0);
    eval 'sub CR1 () {0001000;}' unless defined(&CR1);
    eval 'sub CR2 () {0002000;}' unless defined(&CR2);
    eval 'sub CR3 () {0003000;}' unless defined(&CR3);
    eval 'sub TABDLY () {0014000;}' unless defined(&TABDLY);
    eval 'sub TAB0 () {0000000;}' unless defined(&TAB0);
    eval 'sub TAB1 () {0004000;}' unless defined(&TAB1);
    eval 'sub TAB2 () {0010000;}' unless defined(&TAB2);
    eval 'sub TAB3 () {0014000;}' unless defined(&TAB3);
    eval 'sub XTABS () {0014000;}' unless defined(&XTABS);
    eval 'sub BSDLY () {0020000;}' unless defined(&BSDLY);
    eval 'sub BS0 () {0000000;}' unless defined(&BS0);
    eval 'sub BS1 () {0020000;}' unless defined(&BS1);
    eval 'sub VTDLY () {0040000;}' unless defined(&VTDLY);
    eval 'sub VT0 () {0000000;}' unless defined(&VT0);
    eval 'sub VT1 () {0040000;}' unless defined(&VT1);
    eval 'sub FFDLY () {0100000;}' unless defined(&FFDLY);
    eval 'sub FF0 () {0000000;}' unless defined(&FF0);
    eval 'sub FF1 () {0100000;}' unless defined(&FF1);
    eval 'sub CBAUD () {0010017;}' unless defined(&CBAUD);
    eval 'sub B0 () {0000000;}' unless defined(&B0);
    eval 'sub B50 () {0000001;}' unless defined(&B50);
    eval 'sub B75 () {0000002;}' unless defined(&B75);
    eval 'sub B110 () {0000003;}' unless defined(&B110);
    eval 'sub B134 () {0000004;}' unless defined(&B134);
    eval 'sub B150 () {0000005;}' unless defined(&B150);
    eval 'sub B200 () {0000006;}' unless defined(&B200);
    eval 'sub B300 () {0000007;}' unless defined(&B300);
    eval 'sub B600 () {0000010;}' unless defined(&B600);
    eval 'sub B1200 () {0000011;}' unless defined(&B1200);
    eval 'sub B1800 () {0000012;}' unless defined(&B1800);
    eval 'sub B2400 () {0000013;}' unless defined(&B2400);
    eval 'sub B4800 () {0000014;}' unless defined(&B4800);
    eval 'sub B9600 () {0000015;}' unless defined(&B9600);
    eval 'sub B19200 () {0000016;}' unless defined(&B19200);
    eval 'sub B38400 () {0000017;}' unless defined(&B38400);
    eval 'sub EXTA () { &B19200;}' unless defined(&EXTA);
    eval 'sub EXTB () { &B38400;}' unless defined(&EXTB);
    eval 'sub CSIZE () {0000060;}' unless defined(&CSIZE);
    eval 'sub CS5 () {0000000;}' unless defined(&CS5);
    eval 'sub CS6 () {0000020;}' unless defined(&CS6);
    eval 'sub CS7 () {0000040;}' unless defined(&CS7);
    eval 'sub CS8 () {0000060;}' unless defined(&CS8);
    eval 'sub CSTOPB () {0000100;}' unless defined(&CSTOPB);
    eval 'sub CREAD () {0000200;}' unless defined(&CREAD);
    eval 'sub PARENB () {0000400;}' unless defined(&PARENB);
    eval 'sub PARODD () {0001000;}' unless defined(&PARODD);
    eval 'sub HUPCL () {0002000;}' unless defined(&HUPCL);
    eval 'sub CLOCAL () {0004000;}' unless defined(&CLOCAL);
    eval 'sub CBAUDEX () {0010000;}' unless defined(&CBAUDEX);
    eval 'sub BOTHER () {0010000;}' unless defined(&BOTHER);
    eval 'sub B57600 () {0010001;}' unless defined(&B57600);
    eval 'sub B115200 () {0010002;}' unless defined(&B115200);
    eval 'sub B230400 () {0010003;}' unless defined(&B230400);
    eval 'sub B460800 () {0010004;}' unless defined(&B460800);
    eval 'sub B500000 () {0010005;}' unless defined(&B500000);
    eval 'sub B576000 () {0010006;}' unless defined(&B576000);
    eval 'sub B921600 () {0010007;}' unless defined(&B921600);
    eval 'sub B1000000 () {0010010;}' unless defined(&B1000000);
    eval 'sub B1152000 () {0010011;}' unless defined(&B1152000);
    eval 'sub B1500000 () {0010012;}' unless defined(&B1500000);
    eval 'sub B2000000 () {0010013;}' unless defined(&B2000000);
    eval 'sub B2500000 () {0010014;}' unless defined(&B2500000);
    eval 'sub B3000000 () {0010015;}' unless defined(&B3000000);
    eval 'sub B3500000 () {0010016;}' unless defined(&B3500000);
    eval 'sub B4000000 () {0010017;}' unless defined(&B4000000);
    eval 'sub CIBAUD () {002003600000;}' unless defined(&CIBAUD);
    eval 'sub CMSPAR () {010000000000;}' unless defined(&CMSPAR);
    eval 'sub CRTSCTS () {020000000000;}' unless defined(&CRTSCTS);
    eval 'sub IBSHIFT () {16;}' unless defined(&IBSHIFT);
    eval 'sub ISIG () {0000001;}' unless defined(&ISIG);
    eval 'sub ICANON () {0000002;}' unless defined(&ICANON);
    eval 'sub XCASE () {0000004;}' unless defined(&XCASE);
    eval 'sub ECHO () {0000010;}' unless defined(&ECHO);
    eval 'sub ECHOE () {0000020;}' unless defined(&ECHOE);
    eval 'sub ECHOK () {0000040;}' unless defined(&ECHOK);
    eval 'sub ECHONL () {0000100;}' unless defined(&ECHONL);
    eval 'sub NOFLSH () {0000200;}' unless defined(&NOFLSH);
    eval 'sub TOSTOP () {0000400;}' unless defined(&TOSTOP);
    eval 'sub ECHOCTL () {0001000;}' unless defined(&ECHOCTL);
    eval 'sub ECHOPRT () {0002000;}' unless defined(&ECHOPRT);
    eval 'sub ECHOKE () {0004000;}' unless defined(&ECHOKE);
    eval 'sub FLUSHO () {0010000;}' unless defined(&FLUSHO);
    eval 'sub PENDIN () {0040000;}' unless defined(&PENDIN);
    eval 'sub IEXTEN () {0100000;}' unless defined(&IEXTEN);
    eval 'sub TCOOFF () {0;}' unless defined(&TCOOFF);
    eval 'sub TCOON () {1;}' unless defined(&TCOON);
    eval 'sub TCIOFF () {2;}' unless defined(&TCIOFF);
    eval 'sub TCION () {3;}' unless defined(&TCION);
    eval 'sub TCIFLUSH () {0;}' unless defined(&TCIFLUSH);
    eval 'sub TCOFLUSH () {1;}' unless defined(&TCOFLUSH);
    eval 'sub TCIOFLUSH () {2;}' unless defined(&TCIOFLUSH);
    eval 'sub TCSANOW () {0;}' unless defined(&TCSANOW);
    eval 'sub TCSADRAIN () {1;}' unless defined(&TCSADRAIN);
    eval 'sub TCSAFLUSH () {2;}' unless defined(&TCSAFLUSH);
}
1;
