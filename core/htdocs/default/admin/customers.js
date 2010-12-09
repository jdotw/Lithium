function toggleVis()
{
  var warn;
  warn = document.getElementById("warning");

  if(document.details.custid.disabled)
  {
    document.details.custid.disabled=false;
    document.details.custname.disabled=false;
    document.details.baseurl.disabled=false;
    document.details.submitb.disabled=false;
    warn.style.visibility = "visible";
  }
  else
  {
    document.details.custid.disabled=true;
    document.details.custname.disabled=true;
    document.details.baseurl.disabled=true;
    document.details.submitb.disabled=true;
    warn.style.visibility = "hidden";
  }
}

function addRow()
{
  var ni = document.getElementById('inputTableStart');
  var numi = document.getElementById('startMarker');
  var num = (document.getElementById("startMarker").value - 1) + 2;
  numi.value = num;
  var divIdName = "my"+num+"Div";
  var newdiv = document.createElement('div');
  newdiv.setAttribute("id",divIdName);
  newdiv.innerHTML = num+".<span style=\"position: relative; left: 10px;\"><input name=custname[] id=name"+num+">&nbsp;<a href=\"javascript:;\" onClick=\"removeRow('"+divIdName+"')\">REMOVE</a></span><br /><br />";
  ni.appendChild(newdiv);
}

function removeRow(divNum)
{
  var d = document.getElementById('inputTableStart');
  var olddiv = document.getElementById(divNum);
  d.removeChild(olddiv);
}

function IsEmpty(aTextField) {
  if((aTextField.value.length==0) || (aTextField.value==null))
  {
    return true;
  }
  else 
  {
    return false; 
  }
}

function ValidateForm(form) {

  var pass = true;

  for(var n = 0; n < form.length; n++) {

    if(IsEmpty(form.elements[n])) {
      form.elements[n].style.background = "#FFC0C0";
      form.elements[n].focus();
      pass = false;
    }
    else
    {
      if(form.elements[n].type == "text")
      {
        form.elements[n].style.background = "#FFFFFF";
      }
    }
  }

  return pass;
}

function keyStop(e)
{
  if (!e) var e = window.event;

  if (e.keyCode) var keyHit = e.keyCode;
  else if (e.which) var keyHit = e.which;

  if ((keyHit < 48 || keyHit > 57) && (keyHit < 97 || keyHit > 122) && (keyHit != 8) && (keyHit != 127)) {
    return false; }
}
