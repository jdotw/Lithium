<?php

include ("profile.php");
include ("standard.php");

global $customer_id_str;
global $customer_node_ip;
global $customer_guest_username;
global $customer_guest_password;
global $customer_guest_addr;
global $customer_guest_mask;

global $default_node_ip;
global $default_construct_path;
global $default_config_path;
global #default_work_path;
global $default_guest_username;
global $default_guest_password;
global $default_guest_addr;
global $default_guest_mask;

/* Node IP */
if (!$customer_node_ip) $ip = $default_node_ip;
else $ip = $customer_node_ip;

/* Client IP */
$client_ip_str = $_SERVER["REMOTE_ADDR"];
if (!$client_ip_str)
{ printf ("No client IP address specified"); exit; }

/* Guest user/pass/addr/mask */
if (!$customer_guest_username) $guest_username = $default_guest_username;
else $guest_username = $customer_guest_username;
if (!$customer_guest_password) $guest_password = $default_guest_password;
else $guest_password = $customer_guest_password;
if (!$customer_guest_addr) $guest_addr_str = $default_guest_addr;
else $guest_addr_str = $customer_guest_addr;
if (!$customer_guest_mask) $guest_mask_str = $default_guest_mask;
else $guest_mask_str = $customer_guest_mask;

if (!$guest_addr_str)
{ printf ("No guest IP address range is configured"); exit; }

/* Check addr/mask */
$client_ip_int = ip2long ($client_ip_str);
$guest_addr_int = ip2long ($guest_addr_str);
$guest_mask_int = ip2long ($guest_mask_str);

if (($client_ip_int & $guest_mask_int) == ($guest_addr_int & $guest_mask_int))
{
  /* Within guest range, allow guest access */
  standard_start (NULL);

  $line = exec ($default_construct_path . "lithium -n '$ip' -u '$guest_username' -p '$guest_password' -C '$customer_id_str' -r $default_work_path -c $default_config_path -e -q actuator.so a_customer_get_addr_by_id_str $customer_id_str", $ret_array, $ret_val);

  if ($ret_val != 0)
  { print "Encountered an error running actuator (Lithium web interface).<BR><BR>Error code $ret_val<BR>\n"; }

  $res_addr = $ret_array[0];

  $line = exec ($default_construct_path . "lithium -n '$ip' -u '$guest_username' -p '$guest_password' -C '$customer_id_str' -r $default_config_path -e -q actuator.so a_form_get '$res_addr' '$customer_id_str' infstat_main '0'", $ret_array2, $ret_val2);

  if ($ret_val2 != 0)
  { print "Encountered an error running actuator (Lithium web interface).<BR><BR>Error code $ret_val2<BR>\n"; }

  $output = "";
  foreach ($ret_array2 as $row)
  { $output .= $row; }

  print $output;
  standard_end ();

  exit ();
}
else
{ 
  /* Not in guest range, redirect to customer page */
  $server_name = $_SERVER["SERVER_NAME"];
  header("Location: http://$server_name/$customer_id_str"); 
}


?>

