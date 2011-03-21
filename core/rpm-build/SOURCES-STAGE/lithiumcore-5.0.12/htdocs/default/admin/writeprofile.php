<?php

include_once("functions.php");
include("../../include/defaults.php");

global $default_config_path;

if(cf_profilereplace($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "default", $_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "$_POST[custid]", $_POST['custid'], $_POST['custname'], NULL, $_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] ."$_POST[oldid]") && db_custreplace($_POST['custid'], $_POST['custname'], $_POST['baseurl']))
  header("Location: index.php?success=modify");

?>
