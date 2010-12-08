#! /usr/bin/perl
# use module
use XML::Simple;
use Net::DNS;
use Net::IP;
use Data::Dumper;
use Time::HiRes qw(gettimeofday tv_interval);
use Data::Types qw(:all);
use Net::DNS::Resolver;

$NORMAL = "1";
$CONN_REFUSED = "2";
$CONN_TIMEOUT = "3";
$PROTO_TIMEOUT = "4";
$PROTO_ERROR = "5";
$SCRIPT_ERROR = "6";

###################################
#  Script Configuration Variables
###################################

%infostruct = (  'version' => '1.0',
                 'desc'  => "DNS Service Check",
                 'proto' => "DNS",
                 'port'  => "53",
                 'transport' => "UDP",
                 'info'  => "Connects to the device using the DNS (TCP Port 53) protocol
                  and optionally attempts to resolve a specified URL",
                  'config_variable'=> [
                              {
                                 'name' => 'test_hostname',
                                 'desc' => 'Test Hostname',
                                 'required' => '1',
                              },
			     {
				'name' => 'record_type',
				'desc' => 'DNS Record Type (e.g. MX)'
			     },
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
  # 1 Parameter: Filename for the config XML
  my ($filename) = @_;

  # Extract Variables from XML and store in %variables Hash
  %variables = &getVariables($filename);
   
  # Set IP 
  my $ip;
  if (is_string(%variables->{alt_ip}->{value})) { $ip = %variables->{alt_ip}->{value}; }
  else { $ip = %variables->{dev_ip}->{value}; }

  # Set Port
  my $port;
  if (is_string(%variables->{alt_port}->{value})) { $port = %variables->{alt_port}->{value}; }
  else { $port = '53'; }

  # Get Start Time
  $tstart = [gettimeofday];

  #Check if there is a record type, if not use default of A
  $rec_type = %variables->{record_type}->{value};
  if (is_string($rec_type))
  {
    #check if Record type is valid (case insensitive)
    #Valid record types: A, MX, CNAME, PTR, NS, SOA
    if ($rec_type !~ /(?i)^A$|^MX$|^CNAME$|^PTR$|^NS$|^SOA$(?-i)/)
    {
      $status = $SCRIPT_ERROR;
      $message = "Incorrect DNS Record Type";
    }
  }
  else
  {
    #Set record type to default of A
    $rec_type = 'A';
  }

  # Attempt to resolve url
  $handle = Net::DNS::Resolver->new();
  $handle->nameservers($ip);
  my $test_hostname = %variables->{test_hostname}->{value};
  my $packet = $handle->search($test_hostname, $rec_type);
  if ($packet)
  {
    $status = $NORMAL;
    $message = "$test_hostname Resolved for record type: $rec_type";
   }
  else
  {
    $status = $PROTO_ERROR;
    $message = "Failed to resolve hostname: $test_hostname";
  }
  $tresp = [gettimeofday];
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
