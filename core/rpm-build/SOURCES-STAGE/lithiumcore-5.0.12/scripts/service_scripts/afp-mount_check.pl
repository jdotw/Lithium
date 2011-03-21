#!/usr/bin/perl
# use strict;
use XML::Simple;
use Data::Dumper;
use Time::HiRes qw(gettimeofday tv_interval);
use POSIX;
use Data::Types qw(:all);

$NORMAL = "1";
$CONN_REFUSED = "2";
$CONN_TIMEOUT = "3";
$PROTO_TIMEOUT = "4";
$PROTO_ERROR = "5";

###################################
#  Script Configuration Variables
###################################

%infostruct = (  'version' => '0.3.0',
                 'desc'  => "AFP-Service check",
                 'proto' => "AFP",
                 'port'  => "548",
                 'transport' => "TCP",
                 'info' => "Connects to an AFP-Server
                  and tries to mount a givensharepoint",
                  'config_variable'=> [
                              {
                                 'name' => 'afp_user',
                                 'desc' => 'User account',
                                 'required' => '1'
                              },
                              {
                                 'name' => 'afp_passwd',
                                 'desc' => 'User password',
                                 'required' => '1'
                              },                                                            {
                                'name' => 'afp_share',
                                'desc'=> 'Sharepoint to mount',
                                'required' => '1'
                              },
                             {
                                'name' => 'afp_mount_point',
                                'desc'=> 'Path to local mount Point'
                              },
                                                            {
                                'name' => 'error_thrs',
                                'desc'=> 'Warn after ...seconds',
                              }
               ]
            );

%objectstruct = ();

###################################
#  Command processing
###################################

($ARGV[0] eq "info") and ($#ARGV == 0) and
   print &generateXml($xmlroot, \%infostruct);

 
($ARGV[0] eq "object") and ($#ARGV == 0) and
   print &generateXml($xmlroot, \%objectstruct);
   
 
($ARGV[0] eq "check") and ($#ARGV != 0) and
  checkService($ARGV[$#ARGV]);
  
###################################
# Service Check/Testing
###################################

sub checkService
{
  	
	# set the environment for Oracle Instant client
	$ENV{ORACLE_HOME}="/opt/oracle/instantclient";
	$ENV{DYLD_LIBRARY_PATH}="/opt/oracle/instantclient";

	# 1 Parameter: Filename for the config XML
	my ($filename) = @_;
	my $sec;
	my $min;
	my $hour;
	my $mday;
	my $mon;
	my $year;
	my $wday;
	my $yday;
	my $isdst;



	# Extract Variables from XML and store in %variables Hash
	%variables = &getVariables($filename);
   
	# Set IP 
	my $ip;
	if (is_string(%variables->{alt_ip}->{value})) { $ip = %variables->{alt_ip}->{value}; }
	else { $ip = %variables->{dev_ip}->{value}; }

	# Set Port
	my $port;
	if (is_string(%variables->{alt_port}->{value})) { $port = %variables->{alt_port}->{value}; }
	else { $port = '548'; }

    print STDERR "IP: $ip Port: $port";

	# Get Start Time
	$tstart = [gettimeofday];

	# gather credentials und sharepoint name
	my $afp_user = %variables->{afp_user}->{value};
    my $afp_passwd = %variables->{afp_passwd}->{value};
    my $afp_share = %variables->{afp_share}->{value};
    my $afp_mount_point = "";
    my $afp_local_mount = "";
    if (is_string(%variables->{afp_mount_point}->{value})) { $afp_mount_point = %variables->{afp_mount_point}->{value}; }
	else { $afp_mount_point = 'auto'; }

	if (is_string(%variables->{error_thrs}->{value})) { $error_thrs = %variables->{error_thrs}->{value}; }
	else { $error_thrs = ''; }

	# Now check the AFP service by mounting the specified share
	($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);
	$timestamp = sprintf("%04d%02d%02d%02d%02d",$year+1900,$mon+1,$mday,$min,$sec);

	if ($afp_mount_point eq "auto") {
		# Auto-mode is enabled
		# create the mount point on the fly
		$afp_local_mount = "/tmp/lithium_afp_mount_test_".$afp_share."_".$timestamp;
	} else {
		$afp_local_mount = $afp_mount_point;
	}
	system("mkdir -p $afp_local_mount");

    print STDERR "Mount point is $afp_local_mount";
	
	my $mountTimeStart = gettimeofday;
	my $mountTimeDuration;
	my $mountCommand = "mount_afp afp://$afp_user:$afp_passwd\@$ip:$port/$afp_share $afp_local_mount";
	$result = `$mountCommand`;
	
    print STDERR "Mount result is $result";
	
    # check if the mount succeeded
	# $afp_local_mount will only be present if the mount succeeded
	my $mount_check = `df -h | grep $afp_local_mount`;

    print STDERR "Mount check is $mount_check";
    print STDERR "DF output is " . `df -h`;
	
	# mount did not succeed, check errors
	if ($mount_check eq "") {
		$status = $PROTO_ERROR;
		$message = "Mounting AFP share failed!";
	} else {
		$mountTimeDuration = sprintf("%.2f",(gettimeofday-$mountTimeStart));
		$status = $NORMAL;
		$message = "Mounting AFP share successful in $mountTimeDuration s";
	}

	# apply error threshold if mount operation
	# took too long
    print STDERR "Mount time was $mountTimeDuration";
	if ($error_thrs ne "") {
		if ($mountTimeDuration > $error_thrs) {
			$status=$PROTO_ERROR;
			$message = "Mounting AFP was very slow - possible problem!";
            print STDERR "Mount took too long!";
		}
	}
	
	# for safety reasons unmount the share anyway...
	system("umount $afp_local_mount");
	
	$ttrans = [gettimeofday];
    
	%resultstruct = ('metric'=> [
					{ 'name' => 'status', 'value' => $status, },
                    { 'name' => 'resptime', 'value' => tv_interval($tstart, $tresp), },
                    { 'name' => 'transtime', 'value' => tv_interval($tstart, $ttrans), },
                    { 'name' => 'message', 'value' => $message, },
                  ]);
	print &generateXml($xmlroot, \%resultstruct);
}
###########################################
# Standard Functions
###########################################

sub generateXml
{
  # Create the XML from dictionary/array. 
  # You don't have to make changes to this sub.
  # To make changes to these XML Structures:
  #   Modify %configstruct %objectstruct at the 
  #   begining of this file

  # Parameter 1: Temporary XML File
  my ($xmlrool) = $_[0];
  my (%struct) = %{$_[1]};
  
  my $xml = new XML::Simple(NoAttr=>1, RootName=>"service_script",XMLDecl=>1);
  $data = $xml->XMLout(\%struct);
  return "$data";
}

sub getVariables 
{
  # Parameter: Filename for the XML
  my ($filename) = @_;

  # ------>Collect XML Info Here <----------
  my $xml = new XML::Simple; # create new XML object
  my $data = $xml->XMLin($filename);
  my %variables =%{$data->{variable}};
  return %variables;
}
