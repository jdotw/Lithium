<?php

session_start();

$myloc = $_SERVER[DOCUMENT_ROOT] . $_SESSION['subdir'];

if(!file_exists("../profile.php"))
  header("Location: setup.php?step=1\n\n");

include("../../include/standard.php");

if(file_exists("../profile.php"))
  include("../profile.php");

include_once("functions.php");

global $customer_id_str;

$username = adminauth();

if(!$username)
{
  authheaders();
}
else
{

admin_start();

switch($_GET['success'])
{
  case 'modify':
    echo "<h2 style=\"color: green;\">Modification successful.</h2>";
    break;
  
  case 'setup':
    echo "<h2 style=\"color: green;\">Initial configuration successful.<br /><span style=\"color: red;\">You must restart LITHIUM | Core before proceeding.</span><br /><br />Please click <a href=\"/$customer_id_str/\">here</a> to access LITHIUM | Web.</h2>";
    break;

  default:

}

print <<<EndOfHTML

<br />

<a href="customers.php">Configure Customers</a>
<br /><br />
<a href="config.php">Configure Deployment</a>
<br /><br />
<a href="status.php">Server process status</a>

EndOfHTML;

admin_end();

}

?>
