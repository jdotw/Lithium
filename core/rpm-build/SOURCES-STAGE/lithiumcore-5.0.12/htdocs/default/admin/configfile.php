<?php

@session_start();

include($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "/include/standard.php");
include("../profile.php");
include("functions.php");

global $default_config_path;
global $customer_id_str;

$conf = $default_config_path . "/node.conf";

if($_POST['op'] == 'save')
{
  $f = file_get_contents($conf) or die("Can't open configuration file.");

  $fsize = sizeof($f);

  header("Content-Type: application/force-download\n\n");
  header("Content-Disposition: attachment; filename=lithiumcore.conf");
  header("Content-Length: $fsize");

  echo $f;
}
else if($_POST['op'] == 'replace')
{
  $arr = $_FILES['file']['tmp_name'];
  if(move_uploaded_file($arr, $conf))
  {
    echo "<meta http-equiv=\"refresh\" content=\"2; url=/$customer_id_str/admin/config.php\"></head>";
    admin_start();
    echo"<body>Config Saved.<br />Returning you now.</body></html>";
  }
}
else
  die("Invalid Operation attempted.");
