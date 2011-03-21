#! /usr/bin/perl
# use module
use XML::Simple;
use Net::HTTP;
use HTTP::Request;
use LWP::UserAgent;
use Data::Dumper;
use Time::HiRes qw(gettimeofday tv_interval);
use Data::Types qw(:all);

$NORMAL = "1";
$CONN_REFUSED = "2";
$CONN_TIMEOUT = "3";
$PROTO_TIMEOUT = "4";
$PROTO_ERROR = "5";

###################################
#  Script Configuration Variables
###################################

%infostruct = (  'version' => '1.0',
                 'desc'  => "HTTP Service Check",
                 'proto' => "HTTP",
                 'port'  => "80",
                 'transport' => "TCP",
                 'info'  => "Connects to the device using the HTTP (TCP Port 80) protocol
                  and optionally attempts to download a specified URL",
                  'config_variable'=> [
                              {
                                 'name' => 'test_url',
                                 'desc' => 'Test URL'
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
  else { $port = '80'; }

  # Get Start Time
  $tstart = [gettimeofday];

  # Check TCP Connection and server response
  $tcp = new IO::Socket::INET (PeerAddr => $ip, PeerPort => $port, Proto => 'tcp');
  if ($tcp)
  {
    $tcp->close();
    $status = $NORMAL;
    $message = "Connection established successfully";
  }
  else
  { 
    $status = $CONN_REFUSED; 
    $message = "TCP Connection Refused";
  }
  $tresp = [gettimeofday];
  $ttrans = [gettimeofday];

  if ($status == $NORMAL)
  {
    # Check for test_recipients
    my $test_url = %variables->{test_url}->{value};
    if (is_string($test_url))
    {
      $request = HTTP::Request->new(GET => $test_url);
      $ua = LWP::UserAgent->new;
      $response = $ua->request($request);
      $message = $response->status_line;
      if ($response->is_success)
      {
        $status = $NORMAL;
      }
      else
      {
        $status = $PROTO_ERROR;
      }
      $ttrans = [gettimeofday];
    }
  }

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
