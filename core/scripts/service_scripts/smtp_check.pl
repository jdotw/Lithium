#! /usr/bin/perl
# use module
use XML::Simple;
use Net::SMTP;
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
                 'desc'  => "SMTP Service Check",
                 'proto' => "SMTP",
                 'port'  => "25",
                 'transport' => "TCP",
                 'info' => "Connects to the device using the SMTP protocol
                  and optionally attempts to send a test message using the
                  supplied credentials",
                  'config_variable'=> [
                              {
                                 'name' => 'test_recipients',
                                 'desc' => 'Test Email Recipients'
                              },
                              {
                                'name' => 'test_from',
                                'desc'=> 'Test Email From: Address'
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
  else { $port = '25'; }

  # Get Start Time
  $tstart = [gettimeofday];

  # Check TCP Connection and server response
  $tcp = new IO::Socket::INET (PeerAddr => $ip, PeerPort => '25', Proto => 'tcp');
  if ($tcp)
  {
    $line = <$tcp>;
    if ($line)
    {
      $message = $line;
      if ($line =~ /^2+/)
      { $status = $NORMAL }
      else
      { $status = $PROTO_ERROR; }
    }
    else
    {
      $status = $PROTO_ERROR;
      $message = "No output received from server.";
    }
    $tcp->close();
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
    my $test_recipients = %variables->{test_recipients}->{value};
    my $test_from = %variables->{test_from}->{value};
    if (is_string($test_recipients))
    {
      # test_recipients has been configured
      # Attempt to send a test email
      $smtp = Net::SMTP->new($ip, Timeout => 60);
      if ($smtp)
      {
        # Connected
        if (is_string($test_from))
        { $smtp->mail ($test_from); }
        else
        { $smtp->mail ("lithium"); }
        my @recipients = split (/[,;]/, %variables->{test_recipients}->{value});
        $smtp->recipient (@recipients, { Notify => ['FAILURE','DELAY'], SkipBad => 1});
        $smtp->data();
        $smtp->datasend("To: " . %variables->{recipients}->{value} . "\n");
        $smtp->datasend("Subject: Lithium SMTP Service Check Email\n");
        $smtp->datasend("\n");
        $smtp->datasend("This is a test message.");
        $smtp->dataend();
        $smtp->quit();
        $status = $NORMAL;
      }
      else
      {
        # Failed to connect/HELO/EHLO
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
