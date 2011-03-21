require '_h2ph_pre.ph';

no warnings 'redefine';

unless(defined(&_POSIX_TYPES_H_WRAPPER)) {
    require 'features.ph';
    eval {
	my(@REM);
	my(%INCD) = map { $INC{$_} => 1 } (grep { $_ eq "asm/posix_types.ph" } keys(%INC));
	@REM = map { "$_/asm/posix_types.ph" } (grep { not exists($INCD{"$_/asm/posix_types.ph"}) and -f "$_/asm/posix_types.ph" } @INC);
	require "$REM[0]" if @REM;
    };
    warn($@) if $@;
    if(defined( &__FD_ZERO)  && !defined( &__GLIBC__)) {
	undef(&__FD_ZERO) if defined(&__FD_ZERO);
	eval 'sub __FD_ZERO {
	    my($fdsetp) = @_;
    	    eval q(\\"(assembly code)\\");
	}' unless defined(&__FD_ZERO);
    }
    eval 'sub _POSIX_TYPES_H_WRAPPER () {1;}' unless defined(&_POSIX_TYPES_H_WRAPPER);
}
1;
