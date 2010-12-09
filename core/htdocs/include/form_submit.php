<?php

function a_form_submit ($srcresaddr, $srcentaddr, $srcform, $srcrefsec, $srcpassdata, $dstresaddr, $dstentaddr, $dstform, $dstrefsec, $dstpassdata, $postvars, $ret_val)
{
  $arg = "";
  
  foreach ($postvars as $key => $value) 
  {
    $arg .= "'$key' '$value' ";
  }
  
  return actuate ("a_form_submit", "'$srcresaddr' '$srcentaddr' '$srcform' '$srcrefsec' '$srcpassdata' '$dstresaddr' '$dstentaddr' '$dstform' '$dstrefsec' '$dstpassdata' $arg", $ret_val);
}

?>
