<?php

include ("defaults.php");
include ("actuate.php");
include ("xml_get.php");
include ("form_get.php");
include ("form_submit.php");
include ("link.php");
include ("customer.php");
include ("auth.php");

/* Assignments to remove need for register_globals to be ON
   Should be safe to run with it assigned to OFF (good!) */

if(isset($_GET['action']))
{
  $action = $_GET['action'];
  $resaddr = $_GET['resaddr'];
  $entaddr = $_GET['entaddr'];
  $formname = $_GET['formname'];
  $refsec = $_GET['refsec'];
  $passdata = $_GET['passdata'];

  $srcresaddr = $_GET['srcresaddr'];
  $srcentaddr = $_GET['srcentaddr'];
  $srcresent = $_GET['srcresent'];
  $srcform = $_GET['srcform'];
  $srcpassdata = $_GET['srcpassdata'];
  $srcrefsec = $_GET['srcrefsec'];
  $dstresaddr = $_GET['dstresaddr'];
  $dstentaddr = $_GET['dstentaddr'];
  $dstform = $_GET['dstform'];
  $dstrefsec = $_GET['dstrefsec'];
  $dstpassdata = $_GET['dstpassdata'];
}
else
{
  $action = '';
}

/* End register_globals assignments */


function standard_menu ($menu_res)
{
  global $customer_id_str;
  global $customer_name;
  
  if ($customer_id_str == "admin")
  {
    $customers_link = link_form_get ("::4::", "customer_list");
    $resources_link = link_form_get ("::0::", "resource_list");
    $conf_link = link_form_get ("::0::", "node_config");
    
    print "<TD><A HREF=$customers_link>Customers</A></TD>\n";
    print "<TD><A HREF=$resources_link>Resources</A></TD>\n";
    print "<TD><A HREF=$conf_link>Node Config</A></TD>\n";
  }
  else
  {
    $addr_str = customer_resaddr ($customer_id_str);
    
    $home_link = link_form_get ($addr_str, "main");
    $site_link = link_form_get ($addr_str, "site_list");
    $users_link = link_form_get ($addr_str, "user_list");
    $ipregistry_link = link_form_get ($addr_str, "ipregistry_networklist");
    $inventory_link = link_form_get ($addr_str, "inventory_vendorlist");
    $incident_link = link_form_get ($addr_str, "incident_list");
    $case_link = link_form_get ($addr_str, "case_main");
    
    print "<TD><A HREF=index.php>Main</A></TD>\n";
	print "<TD><A HREF=$site_link>Sites</A></TD>\n";
	print "<TD><A HREF=$incident_link>Incidents</A></TD>\n";
	print "<TD><A HREF=$case_link>Cases</A></TD>\n";
    print "<TD><A HREF=$users_link>Users</A></TD>\n";
	print "<TD><A HREF=http://support.lithiumcorp.com>Documentation</A></TD>\n";
  }

}

function nav_menu ($menu_res, $menu_ent)
{
  if ($menu_res)
  {
    $res_specific_menu = actuate ("a_form_get", "'$menu_res' '$menu_ent' 'navigation_menu' '0'", $ret_val);
    print $res_specific_menu;
  }
}

function summary_menu ($customer_id_str)
{
  $addr_str = customer_resaddr ($customer_id_str);
      
  $menu = actuate ("a_form_get", "'$addr_str' '$customer_id_str' 'summary' '0'", $ret_val);
  if (!strstr($menu, "error")) print "$menu";
}

function standard_start ($menu_res, $menu_ent)
{
  global $customer_id_str;
  global $customer_name;

  $username = $_SERVER['PHP_AUTH_USER'];

  $version = lithium_version ();
  $subcust = substr($_SERVER['REQUEST_URI'], 0, strrpos($_SERVER['REQUEST_URI'], '/'));
  $lithroot = substr($subcust, 0, strpos($subcust, '/', 1) + 1);

  $css = $lithroot . "site.css";
  $images = $lithroot . "images";

  print <<<END_1
  <?xml version="1.0" encoding="utf-8"?>
  <!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" 
    "http://www.w3.org/TR/html4/loose.dtd">
  <HTML>

    <HEAD>
      <META Http-Equiv="Cache-Control" Content="no-cache">
      <META Http-Equiv="Pragma" Content="no-cache">
      <META Http-Equiv="Expires" Content="0">
      <link rel="stylesheet" href="$css" type="text/css" title="l47" media="all">

END_1;
  
echo "    <TITLE>LITHIUM|Web - $customer_name</TITLE>";

print <<<END_2

  </head>
  
    <BODY>

      <!-- Logo -->
      <DIV ID=logo align=center>
        <TABLE CELLPADDING=0 CELLSPACING=0 WIDTH=950 HEIGHT="100%">
          <TR>
            <TD ID=topleft>&nbsp;</TD>
            <TD ID=topright>Version $version</TD>
          </TR>
          <TR>
            <TD ID=weblogo>
            <img src="$images/WebLogo.gif" alt="Web Logo">
            </TD>
            <TD ID=symbols>
		      <IMG SRC="$images/WebIcons.gif">
	        </TD>
	      </TR>
	      <TR>
	        <TD ID=bottomcenter COLSPAN=2>
              $username@$customer_name
            </TD>
          </TR>
        </TABLE>
      </DIV>
      <!-- END Logo -->

      <!-- Menu Bar -->
      <DIV ID=menu align="center">
        <TABLE WIDTH=950>
          <TR>
END_2;

  if ($menu_res) 
  { standard_menu ($menu_res); }  

  print ("</TR>
        </TABLE>
      </DIV>
      <!-- END Menu Bar -->

      <!-- Content Area -->
      <DIV ID=content align=\"center\">
        <TABLE WIDTH=950>
          <TR>
            <TD ID=nav>
              <!-- Navigation Pane -->\n");

  nav_menu ($menu_res, $menu_ent);
  summary_menu ($customer_id_str);

  print ("  
            <!-- END Navigation Pane -->
            </TD>
            <TD ID=main>
              <!-- Main Content Pane -->");

}

function standard_end ()
{
  global $customer_id_str;
  global $customer_name;
 
  print ("    <!-- END Main Content Pane -->
            </TD>
          </TR>
        </TABLE>
      </DIV>
      <!-- END Content Area -->

    </BODY>
    </HTML>\n");
}

?>
