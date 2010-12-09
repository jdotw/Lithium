<?php

session_start();

switch($_GET['step'])
{
  case 1:
    getCustomerInfo();
    break;

  case 2:
    getConfigInfo();
    break;

  case 3:
    writeSetupInfo();
    break;

  default;
    header("Location: http://" . $_SERVER[SERVER_NAME] . "/default/admin/index.php");
}

function getCustomerInfo()
{
  include('functions.php');
  require("customers.php");
  
}

function getConfigInfo()
{
  include('functions.php');
  require("config.php");
}

function writeSetupInfo()
{

  $_SESSION['custid'] = $_POST['custid'];

  include('functions.php');
  require($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "include/defaults.php");
  require($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "include/actuate.php");

  admin_start();

  echo "<h2>Configuring...</h2><blockquote>";
    

  echo "Adding global configuration file... ";

  if(cf_nodereplace($_POST['nodename'], $_POST['adminusername'], $_POST['adminpass'], $_POST['dbuser'], $_POST['dbpass'], $_POST['dbhost'], $_POST['dbport'], $_POST['authexternal']))
    echo "<span style=\"color: green;\"><b>done.</b></span><br />";
  else
    echo "<span style=\"color: red;\"><b>failed.</b></span><br />";

  $i = 0;
  
  foreach($_SESSION['custname'] as $k => $v)
  {
    echo "Adding database entry for customer $v... ";
    if(db_custreplace($_POST['custid'][$i], $v, $_POST['vhostname'] . $_POST['custid'][$i]))
      echo "<span style=\"color: green;\"><b>done.</b></span><br />";
    else
    {
      echo "<span style=\"color: red;\"><b>failed.</b></span><br />";
      continue;
    }
    
    echo "Copying system files for customer $v... ";
    if(cf_profilecreate_msp($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "/default", $_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . $_POST['custid'][$i], $_POST['custid'][$i], $v, TRUE))
      echo "<span style=\"color: green;\"><b>done.</b></span><br />";
    else
    {
      echo "<span style=\"color: red;\"><b>failed.</b></span><br />";
      continue;
    }
  $i++;
  }

  echo "<h3>Finished.</h3>";
    
  echo "</blockquote>";

  /*
 print <<<END1

<div id=redbox>
  <h2>You must now restart LITHIUM|Core</h2> 
  <blockquote>
    <h3>On Mac OS X run the following command in Terminal.app:</h3>
    <div id=command>sudo /Library/StartupItems/Lithium/Lithium restart</div>
    <h3>On Linux run the following command as root:</h3>
    <div id=command>/etc/init.d/lithium restart</div>
  </blockquote>
<p><p>
</div>
<br /><br />
*/

 print <<<END1
 <div>
  <h2>Attempting to automatically re-start LITHIUM|Core...</h2>
  <blockquote>

END1;

 $f = file($default_config_path . "/node.conf") or die("Can't open config file for reading.");
 $un = trim(str_replace(array("username ", '"'), "", $f[10]));
 $pw = trim(str_replace(array("password ", '"'), "", $f[11]));
 $line = exec ($default_construct_path . "lithium -n '127.0.0.1' -u '$un' -p '$pw' -C 'admin' -r $default_work_path -c $default_config_path -m $default_module_path -e -q actuator.so a_form_get '::4:0:' '' restart '' ''", $ret_array, &$ret_val);
 if ($ret_val == 0)
 {
   /* Success */
   print "<div id=greenbox><h3>LITHIUM|Core Successfully re-started</h3></div>";
   print <<<END1

  </blockquote>
 </div>
 
 <div>
   <h2>Proceed to LITHIUM|Web...</h2>
  <blockquote>
     <div id=greenbox>

END1;

 }
 else
 {
   print <<<END2

   <div id=redbox>
     <h3>Automatic re-start of LITHIUM|Core FAILED.</h3> 
     You must now manually re-start LITHIUM|Core using one of the following methods: 
       <blockquote>
       <h3>On Mac OS X run the following command in Terminal.app:</h3>
       <div id=command>sudo /Library/StartupItems/Lithium/Lithium restart</div>
       <h3>On Linux run the following command as root:</h3>
       <div id=command>/etc/init.d/lithium restart</div>
       </blockquote>
    </div>

  </blockquote>
 </div>

 <div>
   <h2>After re-starting LITHIUM|Web...</h2>
  <blockquote>
     <div id=greenbox>

END2;

 }

  $i = 0;
  foreach($_SESSION['custname'] as $k => $v)
  {
    echo "<h3>Click <a href=\"" . $_SESSION['subdir'] . $_POST['custid'][$i] . "\">here</a> to proceed to LITHIUM|Web ($v).</h3>";
    $i++;
  }

  echo "</div></blockquote><br /><br />";

  echo "<p><b>Click <a href=\"$_SESSION[subdir]default/admin/index.php\">here</a> to proceed to LITHIUM|Core Admin Interface.</b></p>";

print <<<END1

  </blockquote>
  </div>
  </div>
END1;

  admin_end();
}

?>
