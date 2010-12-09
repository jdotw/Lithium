<?php

function a_xml_get ($resaddr, $entaddr, $xmlname, $refsec, &$ret_val)
{
  if ($_FILES['xmlfile'] && is_uploaded_file($_FILES['xmlfile']['tmp_name']))
  { $xmlout = $_FILES['xmlfile']['tmp_name']; }
  else
  { $xmlout = ""; }

  return actuate ("a_xml_get", "'$resaddr' '$entaddr' '$xmlname' '$refsec' '$xmlout'", &$ret_val);
}

?>
