<?php

function authenticate_no_connection ()
{
  /* This is called when the authenticate function
   * was unable to connect to the Lithium client_handler
   */

  print "Lithium is currently unavailable whilst maintenance is performed.<BR>";
}

function authenticate_headers ()
{
  global $customer_id_str;
  
  header("WWW-Authenticate: Basic realm=$customer_id_str");
  header("HTTP/1.0 401 Unauthorized");
  echo "You must provide a login and password";
}

function authenticate ()
{
  global $customer_id_str;
  global $customer_node_ip;
  global $default_node_ip;
  global $default_construct_path;
  
  if (!isset($_SERVER["PHP_AUTH_USER"])) 
  {
    /* No username/password present */
    authenticate_headers ();
    exit;
  } 
  
  /* Username/password present */
  
  $username = $_SERVER["PHP_AUTH_USER"];
  $password = $_SERVER["PHP_AUTH_PW"];    
}

function auth_required ($custaddr)
{
  actuate ("a_auth_required", "'$custaddr'", &$ret_val);
  if ($ret_val == 5) return false;
  else return true;
}
?>
