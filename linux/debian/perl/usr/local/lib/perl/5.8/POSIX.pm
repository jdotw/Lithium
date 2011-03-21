package POSIX;

our(@ISA, %EXPORT_TAGS, @EXPORT_OK, $AUTOLOAD) = ();

our $VERSION = "1.09";

use AutoLoader;

use XSLoader ();

# Grandfather old foo_h form to new :foo_h form
my $loaded;

sub import {
    load_imports() unless $loaded++;
    my $this = shift;
    my @list = map { m/^\w+_h$/ ? ":$_" : $_ } @_;
    local $Exporter::ExportLevel = 1;
    Exporter::import($this,@list);
}

sub croak { require Carp;  goto &Carp::croak }
# declare usage to assist AutoLoad
sub usage;

XSLoader::load 'POSIX', $VERSION;

my %NON_CONSTS = (map {($_,1)}
                  qw(S_ISBLK S_ISCHR S_ISDIR S_ISFIFO S_ISREG WEXITSTATUS
                     WIFEXITED WIFSIGNALED WIFSTOPPED WSTOPSIG WTERMSIG));

sub AUTOLOAD {
    if ($AUTOLOAD =~ /::(_?[a-z])/) {
	# require AutoLoader;
	$AutoLoader::AUTOLOAD = $AUTOLOAD;
	goto &AutoLoader::AUTOLOAD
    }
    local $! = 0;
    my $constname = $AUTOLOAD;
    $constname =~ s/.*:://;
    if ($NON_CONSTS{$constname}) {
        my ($val, $error) = &int_macro_int($constname, $_[0]);
        croak $error if $error;
        *$AUTOLOAD = sub { &int_macro_int($constname, $_[0]) };
    } else {
        my ($error, $val) = constant($constname);
        croak $error if $error;
	*$AUTOLOAD = sub { $val };
    }

    goto &$AUTOLOAD;
}

package POSIX::SigAction;

use AutoLoader 'AUTOLOAD';
sub new { bless {HANDLER => $_[1], MASK => $_[2], FLAGS => $_[3] || 0, SAFE => 0}, $_[0] }

package POSIX;

1;
