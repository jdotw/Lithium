<?php

  session_start();

  /*
    Let's try and find out where we are. 
  */

  unset($_SESSION['subdir']);
  
    if(strpos($_SERVER['SCRIPT_NAME'], 'admin') && ereg('/.+/admin/', $_SERVER['SCRIPT_NAME']))
    {
      $_SESSION['subdir'] = str_replace('//', '/', substr($_SERVER['SCRIPT_NAME'], 0, strpos($_SERVER['SCRIPT_NAME'], 'default/admin')));
    }
    else
    {
      $_SESSION['subdir'] = str_replace('//', '/', substr($_SERVER['REQUEST_URI'], 0, strrpos($_SERVER['REQUEST_URI'], '/') + 1));
    }

include($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "include/defaults.php");


/**
 * Authentication for the Administration Area
 *
 * The adminauth() function reads its username/password pair from the appropriate
 * clause in the node.conf
 */
function adminauth()
{
  include_once($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "/include/standard.php");
  include($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "/include/defaults.php");
  include("../profile.php");

  global $default_config_path;
  global $customer_name;
	
  $f = file($default_config_path . "/node.conf") or die("Can't open config file for reading.");

  $un = trim(str_replace(array("username ", '"'), "", $f[10]));
  $pw = trim(str_replace(array("password ", '"'), "", $f[11]));

  if(!isset($_SERVER[PHP_AUTH_USER]))
  {
    authheaders();
  }
  if($_SERVER[PHP_AUTH_USER] == $un && $_SERVER[PHP_AUTH_PW] == $pw)
    return $un;
  else
  {
    return FALSE;
  }
}

/**
 * Headers for HTTP Authentication
 *
 * HTTP Basic Authentication headers, called by multiple functions.
 */
function authheaders()
{
  header("WWW-Authenticate: Basic realm=\"$customer_name Lithium Administration\"");
  header("HTTP/1.0 401 Unauthorized");
  echo("Authentication required for the Lithium Administration Area. $un and $pw");
  exit;
}

/**
 * Replace customer entry in the 'customers' table, part of the base 'lithium' database.
 *
 * id: Customer ID in the 'customers' relation.
 */
function db_custreplace($id, $name, $baseurl)
{
  include($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "include/defaults.php");

  $db = cf_getdb('lithium');

  if(!@pg_query("SELECT * from customers"))
  {
    $c = pg_query("CREATE TABLE customers (name varchar, descr varchar, baseurl varchar)");
  }

  if(@pg_query("DELETE FROM customers WHERE name = '$id'"))
  {
    $q = pg_query("INSERT INTO customers(name, descr, baseurl) VALUES('$id', '$name', '$baseurl')");
    return true;
  }
}

/**
 * Replace a customer profile.php configuration file.
 *
 * templateDir: Location of the 'default' directory skeleton.
 * symlink: name of new symlink - should equal custid
 * custid: Customer ID
 * custname: Customer Description
 * isnew: Boolean to indicate if we are creating a new or replacing an old customer
 * oldid: if isnew === FALSE, oldid must be nominated
 */
function cf_profilereplace($templateDir, $symlink, $custid, $custname, $isnew, $oldid=NULL)
{
  if(!isset($isnew))
  {
    if(is_link($oldid))
      unlink($oldid);
    else
     die("Can't remove old symlink - $oldid.");
  }

  if(!is_link($symlink))
    symlink($templateDir, $symlink) or die("Can't create symlink.");

  $contents = stripslashes("\<\?php\n\n");

  $contents .= '$customer_id_str = ';
  $contents .= stripslashes("\"$custid\"\;\n");

  $contents .= '$customer_name = ';
  $contents .= stripslashes("\"$custname\"\;\n");

  $contents .= stripslashes("\n\?\>\n");

  if(($res = fopen($symlink . "/profile.php", w)) && fwrite($res, $contents))
  {
    fclose($res);
    return true;
  }

}

function cf_profilecreate_msp($templateDir, $custdir, $custid, $custname, $isnew=FALSE, $oldid=NULL)
{
  if($isnew)
  {
    if(is_dir($templateDir) && !file_exists($custdir))
    {
      mkdir($custdir) or die("Can't make customer dir $custdir"); 
      mkdir($custdir . "/cache"); 
      mkdir($custdir . "/image_cache");

      symlink($templateDir . "/admin", $custdir . "/admin"); 
      symlink($templateDir . "/index.php", $custdir . "/index.php"); 
      symlink($templateDir . "/xml.php", $custdir . "/xml.php");
      symlink($templateDir . "/diag_auth.php", $custdir . "/diag_auth.php");
      symlink($templateDir . "/diag_res.php", $custdir . "/diag_res.php");
      symlink($templateDir . "/console.php", $custdir . "/console.php");
      symlink($templateDir . "/script.php", $custdir . "/script.php");
    }
    else
    {
      lithium_error("<b>Invalid Template Directory or target exists.</b><br /><br /> Template Directory:  $templateDir<br />Target Directory: $custdir", "0x00004001");
    }
  }
  else
  {
    if(is_dir($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "/$oldid") && rename($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "/$oldid", $custdir))
      $success = true;
    else
      lithium_error("Cannot move old directory $oldid. target is $custdir", "0x00004002");
  }

  $contents = stripslashes("\<\?php\n\n");

  $contents .= '$customer_id_str = ';
  $contents .= stripslashes("\"$custid\"\;\n");

  $contents .= '$customer_name = ';
  $contents .= stripslashes("\"$custname\"\;\n");

  $contents .= stripslashes("\n\?\>\n");

  if(($res = fopen($custdir . "/profile.php", w)) && fwrite($res, $contents))
  {
    fclose($res);
    $success = true;
  }

  if(touch($templateDir . "/profile.php") && $success)
    $success = true;
  else
    $success = false;

  return $success;
}
    

function cf_nodereplace($custid, $adminuser, $adminpass, $dbuser, $dbpass, $dbhost, $dbport, $authexternal)
{
  include($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "include/defaults.php");

  if(empty($adminuser))
    lithium_error("Empty admin username. Please go back and add it.", "0x00004003");

  if(empty($adminpass))
    die("Empty admin password. Please go back and add it.");

  if(empty($dbuser))
    die("Empty database username. Please go back and add it.");

  if(empty($dbpass))
    die("Empty database password. Please go back and add it.");

  if(empty($dbhost))
    die("Empty database hostname. Please go back and add it.");

  if(empty($dbport))
    die("Empty database port. Please go back and add it.");

  $res = fopen($default_config_path . "/node.conf", w) or die("Can't open config file for writing.");

  $contents = "<section id>\n";
  $contents .= stripslashes("  plexus \"$custid\"\n");
  $contents .= stripslashes("  node \"$custid\"\n");
  $contents .= "</id>\n\n";

  $contents .= stripslashes("<section debug>\n  level \"1\"\n</debug>\n\n");

  $contents .= "<section master_user>\n";
  $contents .= stripslashes("  username \"$adminuser\"\n");
  $contents .= stripslashes("  password \"$adminpass\"\n");
  $contents .= "</master_user>\n\n";

  $contents .= "<section postgresql>\n";
  $contents .= stripslashes("  username \"$dbuser\"\n");
  $contents .= stripslashes("  password \"$dbpass\"\n");
  $contents .= stripslashes("  host \"$dbhost\"\n");
  $contents .= stripslashes("  port \"$dbport\"\n");
  $contents .= "</postgresql>\n\n";

  $contents .= stripslashes("<section fonts>\n  rrdfont \"$default_work_path/fonts/rrdfont.ttf\"\n</fonts>\n\n");

  $contents .= stripslashes("<section httpd>\n  imageroot \"$_SERVER[DOCUMENT_ROOT]$_SESSION[subdir]\"\n</httpd>\n\n");

  $contents .= stripslashes("<section authentication>\n  external \"$authexternal\"\n</authentication>\n");

  if(fwrite($res, $contents))
  {
    fclose($res);
    return true;
  }
}


function cf_getdb($dbname)
{
  include($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "include/defaults.php");

  $f = @file($default_config_path . "/node.conf") or die ("Can't open config file " . $path . "/node.conf for reading.");

  $dbuser = trim(str_replace(array("username ", '"'), "", $f[15]));
  $dbpass = trim(str_replace(array("password ", '"'), "", $f[16]));
  $dbhost = trim(str_replace(array("host ", '"'), "", $f[17]));
  $dbport = trim(str_replace(array("port ", '"'), "", $f[18]));

  $dbconnect = "user=$dbuser password=$dbpass host=$dbhost port=$dbport dbname=$dbname";

  $connection = pg_connect($dbconnect);
  
  if (!$connection)
  {
    print <<<END_1

<br />
<div id=errorbox>
  <h2>Unable to connect to PostgreSQL Database Server</h2>
  <p>Please check the following:
  <ul>
    <li>PostgreSQL is <b>installed</b> on your machine
    <li>PostgreSQL is <b> configured for password authentication</b>
    <li>The PostgreSQL database processes are <b>running</b> (postmaster)
    <li>The database authentication credentials are correct
  </ul>
  </p>
  <p>The following crucial installation steps must be completed before this
  stage:
  <ul>
    <li><b>Create 'lithium' PostgreSQL database user</b>
    <li><b>Create 'lithium' PostgreSQL database</b>
  </ul>
  </p>
  <h3>Refer to either the <a href=http://support.lithiumcorp.com/content/category/2/13/70/>Installation Guides</a> 
  or <a href=http://support.lithiumcorp.com/content/category/2/41/75/>Troubleshooting Guides</a> for more information</h3>

  </div>

END_1;

    exit;
  }

  return $connection;

}


function admin_start()
{

  session_start();

  define('_BANNER_OUTPUT', 1);

  print <<<EndOfHTML

  <?xml version="1.0" encoding="utf-8"?>
  <!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" 
    "http://www.w3.org/TR/html4/loose.dtd">
  <HTML>

    <HEAD>
      <link rel="stylesheet" href="$_SESSION[subdir]site.css" type="text/css" title="l47" media="all">
    <TITLE>LITHIUM|Web - Administration Interface</TITLE>
  </head>
  
    <BODY>
      <!-- Logo -->
      <DIV ID=logo align=center>
        <TABLE CELLPADDING=0 CELLSPACING=0 WIDTH=800 HEIGHT="100%">
          <TR>
            <TD ID=topleft>&nbsp;</TD>
            <TD ID=topright>&nbsp;</TD>
          </TR>
          <TR>

            <TD ID=weblogo>
            <img src="$_SESSION[subdir]images/CoreLogo.gif" alt="Core Logo">
            </TD>
            <TD ID=symbols>
		      <IMG SRC="$_SESSION[subdir]images/WebIcons.gif">
	        </TD>
	      </TR>
	      <TR>
	        <TD ID=bottomcenter COLSPAN=2>
		&nbsp;
            </TD>
          </TR>

        </TABLE>
      </DIV>
      <!-- END Logo -->

      <!-- Menu Bar -->
      <DIV ID=menu align="center">
        <TABLE WIDTH=800>
          <TR></TR>
        </TABLE>

      </DIV>
      <!-- END Menu Bar -->

      <!-- Content Area -->
      <DIV ID=content align="center">
        <TABLE WIDTH=800>
          <TR>
            <TD width=40>
              <!-- Navigation Pane -->
              &nbsp;
            <!-- END Navigation Pane -->
            </TD>
            <TD ID=main>
              <!-- Main Content Pane -->

EndOfHTML;

}

function admin_end()
{
  print <<<EndOfHTML

<!-- END Main Content Pane -->
	  </TD>
        </TR>
      </TABLE>
    </DIV>
<!-- END Content Area -->

  </BODY>
</HTML>

EndOfHTML;

}

function lithium_error($message, $code="Unspecified.")
{
  if(!defined('_BANNER_OUTPUT'))
    admin_start();

  echo "<h2>Error Encountered</h2>";

  echo "<h3>Error code: $code</h3>";

  echo $message;

  admin_end();

  exit;
}

function safeCustomerName($in)
{
  return substr(strtolower(trim(str_replace(array('!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '=', '{', '}', '[', ']', ':', ';', '"', '<', '>', "'", ' ', ',', '.', '/', '?', '|', "\\", "~", "`", "-", "_", "+"), "", $in))), 0, 12);
}

?>
