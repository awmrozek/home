var ifrm = document.getElementById("doc_survey");
ifrm.style.display = "none";
window.addEventListener("message", function(evt){
  if(evt.origin.toLowerCase().indexOf('www.getfeedback.com') >=0)
  {
  ifrm.style.display = "";
  }
});


// Copyright 2018 The MathWorks, Inc.
