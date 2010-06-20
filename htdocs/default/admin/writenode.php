<?php

include_once("functions.php");
include("../../include/defaults.php");

global $default_config_path;

if(cf_nodereplace($_POST['nodename'], $_POST['adminusername'], $_POST['adminpass'], $_POST['dbuser'], $_POST['dbpass'], $_POST['dbhost'], $_POST['dbport'], "0"))
  header("Location: index.php?success=modify");

?>
