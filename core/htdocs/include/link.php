<?php

function link_form_get ($resaddr, $formname)
{
  $resaddr_url = rawurlencode ($resaddr);
  $formname_url = rawurlencode ($formname);

  $url = "index.php?action=form_get&resaddr=$resaddr_url&formname=$formname_url";

  return $url;
}

?>
