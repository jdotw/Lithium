<?php

function lithium_version ()
{
  global $default_construct_path;
  global $default_config_path;
	
  $line = exec ($default_construct_path . "lithium -V -r '$default_config_path'", $ret_array, $ret_val);
  if ($ret_val != 0) $line = "Unknown Version";

  return $line;
}

function actuate ($funct, $args, &$ret_val)
{
  include ("defaults.php");

  global $customer_id_str;
  global $customer_node_ip;

  if (!$customer_node_ip) $ip = $default_node_ip;
  else $ip = $customer_node_ip;

  $username = $_SERVER["PHP_AUTH_USER"];
  $password = $_SERVER["PHP_AUTH_PW"];

//  printf("EXEC: ". $default_construct_path . "lithium -n '$ip' -u '$username' -p '$password' -C '$customer_id_str' -r '$default_work_path' -c '$default_config_path' -m '$default_module_path' -e -q actuator.so $funct $args");

  $line = exec ($default_construct_path . "lithium -n '$ip' -u '$username' -p '$password' -C '$customer_id_str' -r '$default_work_path' -c '$default_config_path' -m '$default_module_path' -e -q actuator.so $funct $args", $ret_array, &$ret_val);

  $output = "";
  foreach ($ret_array as $row)
  {
    $output .= $row;
  }

  return $output;
}	

?>
