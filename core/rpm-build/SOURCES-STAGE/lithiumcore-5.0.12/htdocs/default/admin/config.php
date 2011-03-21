<?php

@session_start();

$myloc = $_SERVER[DOCUMENT_ROOT] . $_SESSION['subdir'];

include($myloc . "include/standard.php");
include($myloc . "include/defaults.php");

if(file_exists("../profile.php"))
  include("../profile.php");

include_once("functions.php");

admin_start();

$_SESSION['custname'] = $_POST['custname'];

echo "<link rel=stylesheet type=text/css href=admin.css><script language=javascript src=customers.js></script>";

global $customer_id_str;

$custid = $customer_id_str;

$conffile = $default_config_path . "/node.conf";
$profile = $_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "default/profile.php";

if(empty($_POST['new']))
  echo "<h3 class=\"header\"><a href=\"". $_SESSION['subdir'] ."default/admin/index.php\">Return to menu</a></h3>\n\n";

if(is_writeable($conffile) || (!file_exists($conffile) && is_writeable($default_config_path)))
{
  $wr_conf = '<span style="color: green; font-weight: bold;">Writeable</span>'; $wr_c = true;
}
else
{
  $wr_conf = '<span style="color: red; font-weight: bold;">UNWRITEABLE!</span>'; $wr_c = false;
  $wr_conf_advice = "<blockquote><div id=errorbox>Check the existance and permissions for the $conffile file and the $default_config_path directory.<br /><br />The $default_config_path directory, and the $conffile file (if it exists), <b>MUST be WRITEABLE by the user or group that your installation of apache runs as</b> (typically www, www-data, httpd or apache)<br /><br />Also ensure that if your system supports <b>SE-Linux</b> that you have configured SE-Linux for <b>PERMISSIVE or DISABLED</b> operation.<br /><br />Refer to <a href=http://support.lithiumcorp.com/content/category/2/41/75/>Troubleshooting Guide</a> for more information.</div></blockquote>";
}

if(!file_exists($profile) || (file_exists($profile) && is_writeable($profile)))
{
  $wr_profile = '<span style="color: green; font-weight: bold;">Writeable</span>'; $wr_p = true;
}
else
{
  $wr_profile = '<span style="color: red; font-weight: bold;">UNWRITEABLE!</span>'; $wr_p = false;
  $wr_profile_advice = "<blockquote><div id=errorbox>Check the existance and permissions for the $profile file.<br /><br />The file <b>MUST EXIST and be WRITEABLE by the user or group that your installation of apache runs as</b> (typically www, www-data, httpd or apache)<br /><br />Refer to <a href=http://support.lithiumcorp.com/content/category/2/41/75/>Troubleshooting Guide</a> for more information.</div></blockquote>";
}

if(is_writeable($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir]))
{
  $wr_docroot = '<span style="color: green; font-weight: bold;">Writeable</span>'; $wr_d = true;
}
else
{
  $wr_docroot = '<span style="color: red; font-weight: bold;">UNWRITEABLE!</span>'; $wr_d = false;
  $wr_docroot_advice = "<blockquote><div id=errorbox>Check the permissions for the $_SERVER[DOCUMENT_ROOT]$_SESSION[subdir] directory.<br /><br />The directory <b>MUST be WRITEABLE by the user or group that your installation of apache runs as</b> (typically www, www-data, httpd or apache)<br /><br />Refer to <a href=http://support.lithiumcorp.com/content/category/2/41/75/>Troubleshooting Guide</a> for more information.</div></blockquote>";
}

if (extension_loaded('pgsql'))
{
  $mod_pgsql = '<span style="color: green; font-weight: bold;">Enabled</span>'; $m_pg = true;;
}
else
{
  $mog_pgsql = '<span style="color: red; font-weight: bold;">NOT PRESENT</span>'; $m_pg = false;
  $mod_pgsql_advice = "<blockquote><div id=errorbox>PHP does not have PostgreSQL support enabled. <br /><br />You must install the <b>php-pgsql package</b> or re-compile PHP with <b>PostgreSQL database support</b><br /><br />Refer to <a href=http://support.lithiumcorp.com/content/category/2/41/75/>Troubleshooting Guide</a> for more information.</div></blockquote>";
}

// Some default values. Will get overwritten by the file import below if this
// isn't the initial configuration run...

$f[10] = 'admin';
$f[15] = 'lithium';
$f[16] = 'lithium';
$f[17] = 'localhost';
$f[18] = '5432';

if($_GET['step'] != 2)
{

$username = adminauth();

$f = file($conffile) or die("Can't open config file.");

$f[10] = trim(str_replace(array("username ", '"'), "", $f[10]));
$f[11] = trim(str_replace(array("password ", '"'), "", $f[11]));
$f[15] = trim(str_replace(array("username ", '"'), "", $f[15]));
$f[16] = trim(str_replace(array("password ", '"'), "", $f[16]));
$f[17] = trim(str_replace(array("host ", '"'), "", $f[17]));
$f[18] = trim(str_replace(array("port ", '"'), "", $f[18]));
$authexternal = trim(str_replace(array("external ", '"'), "", $f[30]));

print <<<EndOfHTML

<div style="background: #c0c0c0; border: 1px solid #808080; padding: 8px;">
<h2>Backup Configuration</h2>
<form name="config" action="configfile.php" method="post">
<input type="hidden" name="op" value="save">
<input type="submit" value="Save Config">
</form>
</div>

<br />
<div style="background: #c0c0c0; border: 1px solid #808080; padding: 8px;">
<h2>Restore Configuration</h2>
<form enctype="multipart/form-data" name="config" action="configfile.php" method="post">
<input type="hidden" name="op" value="replace">
<input type="file" name="file">
<br /><br />
<input type="submit" value="Replace Config">
</form>
</div>

EndOfHTML;

}
else
{
  echo "<h1>Configuration: Step 2</h1>\n\n";

  if (!empty($_POST['new']))
  {
    $target = "setup.php?step=3";
    $onsubmit = "onSubmit=\"javascript:return ValidateForm(this);\"";
  }
  else
  {
    $target = "writenode.php";
  }

  echo "<form name=\"config\" action=\"$target\" method=\"post\" $onsubmit>";

  echo "<blockquote><h3>Adding customers:</h3>Customer Description<span style=\"position: relative; left: 171px;\">Customer Name</span><br />";

  foreach($_POST['custname'] as $i => $v)
  {
    $id = safeCustomerName($v);

    if($id == 'default' || $id == 'images' || $id == 'include')
      $id = $id . "1";

    if($id == '')
      $id = "nosuggestion";
  
    echo "<input type=text name=custname[] value=\"$v\" size=20 disabled=true> <input type=text name=custid[] size=20 value=$id onKeyPress=\"javascript:return keyStop(event);\"><br />";

    $_SESSION['custid'][$i] = $id;
  }

  $ref = array_unique($_SESSION['custid']);
  if(sizeof($ref) != sizeof($_SESSION['custid']))
  {
    lithium_error("Duplicate Customer IDs have been specified. Please go back and correct this.", "0x00002002");
    exit;
  }
}

echo "</blockquote><br />";

print <<<EndOfHTML

<div>
<h2>System Check</h2>
<p>
Config File ($conffile): $wr_conf
$wr_conf_advice
</p>
<p>
Profile ($profile): $wr_profile
$wr_profile_advice
</p>
<p>
Document Root ($_SERVER[DOCUMENT_ROOT]$_SESSION[subdir]): $wr_docroot
$wr_docroot_advice
</p>
<p>
PHP with PostgreSQL Support: $mod_pgsql
$mod_pgsql_advice
</p>
</div>

EndOfHTML;

if (!$wr_c || !$wr_p || !$wr_d || !$m_pg)
{
  echo "<br /><div id=errorbox><h2>The errors above must be corrected before the
  installation can proceed.</h2></div>";
}

$baseurl = "http://" . $_SERVER[SERVER_NAME] . "/" . $custid;

$nodename = explode(".", php_uname(n));

$retr_custid = $_POST['custid'];
$retr_custname = $_POST['custname'];


if (!empty($_POST['new']))
{ 
  $target = "setup.php?step=3"; 
  $onsubmit = "onSubmit=\"javascript:return ValidateForm(this);\"";
}
else
{ 
  $target = "writenode.php"; 
}

print <<<EndOfHTML

<h2>Basic Setup</h2>
<div style="background: #f0f0f0; border: 1px solid #808080; padding: 8px; padding-top: 0px;">

<form name="config" action="$target" method="post" $onsubmit>
<input type="hidden" name="op" value="edit">
<input type="hidden" name="nodename" value="$nodename[0]">

<br />

<table>
EndOfHTML;

print <<<EndOfHTML

<tr>
  <td>
    <B>Global Admin Username<B>
  </td>
  <td>
    <input type="text" name="adminusername" value="$f[10]">
  </td>
  <td>
    Lithium|Web super-user account name (default is <i>admin</i>)
  </td>
</tr>

<tr>
  <td>
    <B>Global Admin Password<B>
  </td>
  <td>
    <input type="text" name="adminpass" value="$f[11]">
  </td>
</tr>

</table>
</div>

<h2>Advanced Configuration Options</h2>
<div style="background: #f0f0f0; border: 1px solid #808080; padding: 8px; padding-top: 0px;">

<form name="config" action="$target" method="post" $onsubmit>
<input type="hidden" name="op" value="edit">
<input type="hidden" name="nodename" value="$nodename[0]">

<br />

<table>
<tr>
  <td>
    Node Name
  </td>
  <td>
    <input type="text" name="fauxnode" value="$nodename[0]" disabled=true>
  </td>
  <td>
    <i>Automatically set to hostname</i>
  </td>
</tr>
EndOfHTML;

if(!empty($_POST['new']))
{
  print <<<EndOfHTML
<tr>
  <td>
    Web Virtual Hostname
  </td>
  <td>
    <input type="text" name="vhostname" value="http://$_SERVER[HTTP_HOST]$_SESSION[subdir]">
  </td>
  <td>
    The hostname for this web server. <i>Automatically set to current apache
    hostname.</i>
  </td>
</tr>
EndOfHTML;
}

print <<<EndOfHTML

<tr>
 <td>
   PostgreSQL Username
  </td>
  <td>
    <input type="text" name="dbuser" value="$f[15]">
  </td>
  <td>
    The PostgreSQL username Lithium|Core will use to connect to the
    database.
    </td>
</tr>

<tr>
  <td>
    PostgreSQL Password
  </td>
  <td>
    <input type="text" name="dbpass" value="$f[16]">
  </td>
  <td>
    PostgreSQL MUST be configured for trusted or password authentication
  </td>
</tr>

<tr>
  <td>
    PostgreSQL Host
  </td>
  <td>
    <input type="text" name="dbhost" value="$f[17]">
  </td>
  <td>
    Hostname or IP addres of your PostgreSQL server (default is
    <i>localhost</i>)
    </td>
</tr>

<tr>
  <td>
    PostgreSQL TCP Port
  </td>
  <td>
    <input type="text" name="dbport" value="$f[18]">
  </td>
  <td>
    TCP Port PostgreSQL is listening on. (default is <i>5432</i>)
    </td>
</tr>

<tr>
  <td>
    Web Install Root
  </td>
  <td>
    <input type="text" name="imageroot" value="$myloc">
  </td>
</tr>

<tr>
  <td>
    Use External Authentication
  </td>
  <td>
EndOfHTML;

  if ($authexternal == "1")
  { echo "<input type=\"checkbox\" name=\"authexternal\" value=\"1\" checked>"; }
  else
  { echo "<input type=\"checkbox\" name=\"authexternal\" value=\"1\">"; }

print <<<EndOfHTML

  </td>
</tr>

</table>
<br /><br />
</div>

EndOfHTML;

?>

<br />
<input type="submit" value="Write Config File" <?php if (!$wr_c || !$wr_d || !$wr_p || !$m_pg) echo "disabled=true"; ?>>

<?php

print <<<EndOfHTML

<br /><br />
</form>


EndOfHTML;

admin_end();

