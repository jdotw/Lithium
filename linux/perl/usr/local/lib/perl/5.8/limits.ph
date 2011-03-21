require '_h2ph_pre.ph';

no warnings 'redefine';

unless(defined(&_GCC_LIMITS_H_)) {
    eval 'sub _GCC_LIMITS_H_ () {1;}' unless defined(&_GCC_LIMITS_H_);
    unless(defined(&_LIBC_LIMITS_H_)) {
	require 'syslimits.ph';
    }
    unless(defined(&_LIMITS_H___)) {
	eval 'sub _LIMITS_H___ () {1;}' unless defined(&_LIMITS_H___);
	undef(&CHAR_BIT) if defined(&CHAR_BIT);
	eval 'sub CHAR_BIT () { &__CHAR_BIT__;}' unless defined(&CHAR_BIT);
	unless(defined(&MB_LEN_MAX)) {
	    eval 'sub MB_LEN_MAX () {1;}' unless defined(&MB_LEN_MAX);
	}
	undef(&SCHAR_MIN) if defined(&SCHAR_MIN);
	eval 'sub SCHAR_MIN () {(- &SCHAR_MAX - 1);}' unless defined(&SCHAR_MIN);
	undef(&SCHAR_MAX) if defined(&SCHAR_MAX);
	eval 'sub SCHAR_MAX () { &__SCHAR_MAX__;}' unless defined(&SCHAR_MAX);
	undef(&UCHAR_MAX) if defined(&UCHAR_MAX);
	if((defined(&__SCHAR_MAX__) ? &__SCHAR_MAX__ : undef) == (defined(&__INT_MAX__) ? &__INT_MAX__ : undef)) {
	    eval 'sub UCHAR_MAX () {( &SCHAR_MAX * 2 + 1);}' unless defined(&UCHAR_MAX);
	} else {
	    eval 'sub UCHAR_MAX () {( &SCHAR_MAX * 2+ 1);}' unless defined(&UCHAR_MAX);
	}
	if(defined(&__CHAR_UNSIGNED__)) {
	    undef(&CHAR_MIN) if defined(&CHAR_MIN);
	    if((defined(&__SCHAR_MAX__) ? &__SCHAR_MAX__ : undef) == (defined(&__INT_MAX__) ? &__INT_MAX__ : undef)) {
		eval 'sub CHAR_MIN () {0;}' unless defined(&CHAR_MIN);
	    } else {
		eval 'sub CHAR_MIN () {0;}' unless defined(&CHAR_MIN);
	    }
	    undef(&CHAR_MAX) if defined(&CHAR_MAX);
	    eval 'sub CHAR_MAX () { &UCHAR_MAX;}' unless defined(&CHAR_MAX);
	} else {
	    undef(&CHAR_MIN) if defined(&CHAR_MIN);
	    eval 'sub CHAR_MIN () { &SCHAR_MIN;}' unless defined(&CHAR_MIN);
	    undef(&CHAR_MAX) if defined(&CHAR_MAX);
	    eval 'sub CHAR_MAX () { &SCHAR_MAX;}' unless defined(&CHAR_MAX);
	}
	undef(&SHRT_MIN) if defined(&SHRT_MIN);
	eval 'sub SHRT_MIN () {(- &SHRT_MAX - 1);}' unless defined(&SHRT_MIN);
	undef(&SHRT_MAX) if defined(&SHRT_MAX);
	eval 'sub SHRT_MAX () { &__SHRT_MAX__;}' unless defined(&SHRT_MAX);
	undef(&USHRT_MAX) if defined(&USHRT_MAX);
	if((defined(&__SHRT_MAX__) ? &__SHRT_MAX__ : undef) == (defined(&__INT_MAX__) ? &__INT_MAX__ : undef)) {
	    eval 'sub USHRT_MAX () {( &SHRT_MAX * 2 + 1);}' unless defined(&USHRT_MAX);
	} else {
	    eval 'sub USHRT_MAX () {( &SHRT_MAX * 2+ 1);}' unless defined(&USHRT_MAX);
	}
	undef(&INT_MIN) if defined(&INT_MIN);
	eval 'sub INT_MIN () {(- &INT_MAX - 1);}' unless defined(&INT_MIN);
	undef(&INT_MAX) if defined(&INT_MAX);
	eval 'sub INT_MAX () { &__INT_MAX__;}' unless defined(&INT_MAX);
	undef(&UINT_MAX) if defined(&UINT_MAX);
	eval 'sub UINT_MAX () {( &INT_MAX * 2 + 1);}' unless defined(&UINT_MAX);
	undef(&LONG_MIN) if defined(&LONG_MIN);
	eval 'sub LONG_MIN () {(- &LONG_MAX - 1);}' unless defined(&LONG_MIN);
	undef(&LONG_MAX) if defined(&LONG_MAX);
	eval 'sub LONG_MAX () { &__LONG_MAX__;}' unless defined(&LONG_MAX);
	undef(&ULONG_MAX) if defined(&ULONG_MAX);
	eval 'sub ULONG_MAX () {( &LONG_MAX * 2 + 1);}' unless defined(&ULONG_MAX);
	if(defined ( &__STDC_VERSION__)  && (defined(&__STDC_VERSION__) ? &__STDC_VERSION__ : undef) >= 199901) {
	    undef(&LLONG_MIN) if defined(&LLONG_MIN);
	    eval 'sub LLONG_MIN () {(- &LLONG_MAX - 1);}' unless defined(&LLONG_MIN);
	    undef(&LLONG_MAX) if defined(&LLONG_MAX);
	    eval 'sub LLONG_MAX () { &__LONG_LONG_MAX__;}' unless defined(&LLONG_MAX);
	    undef(&ULLONG_MAX) if defined(&ULLONG_MAX);
	    eval 'sub ULLONG_MAX () {( &LLONG_MAX * 2 + 1);}' unless defined(&ULLONG_MAX);
	}
	if(defined ( &__GNU_LIBRARY__) ? defined ( &__USE_GNU) : !defined ( &__STRICT_ANSI__)) {
	    undef(&LONG_LONG_MIN) if defined(&LONG_LONG_MIN);
	    eval 'sub LONG_LONG_MIN () {(- &LONG_LONG_MAX - 1);}' unless defined(&LONG_LONG_MIN);
	    undef(&LONG_LONG_MAX) if defined(&LONG_LONG_MAX);
	    eval 'sub LONG_LONG_MAX () { &__LONG_LONG_MAX__;}' unless defined(&LONG_LONG_MAX);
	    undef(&ULONG_LONG_MAX) if defined(&ULONG_LONG_MAX);
	    eval 'sub ULONG_LONG_MAX () {( &LONG_LONG_MAX * 2 + 1);}' unless defined(&ULONG_LONG_MAX);
	}
    }
} else {
    if(defined(&_GCC_NEXT_LIMITS_H)) {
	eval {
	    my(@REM);
	    my(%INCD) = map { $INC{$_} => 1 } (grep { $_ eq "limits.ph" } keys(%INC));
	    @REM = map { "$_/limits.ph" } (grep { not exists($INCD{"$_/limits.ph"}) and -f "$_/limits.ph" } @INC);
	    require "$REM[0]" if @REM;
	};
	warn($@) if $@;
    }
}
1;
