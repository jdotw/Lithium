<?php

function a_form_get ($resaddr, $entaddr, $formname, $refsec, $passdata, &$ret_val)
{
  return actuate ("a_form_get", "'$resaddr' '$entaddr' '$formname' '$refsec' '$passdata'", &$ret_val);
}

?>
