//
// Change validator icons to normal version if highligt is true, otherwise
// change to pale icon
//
function activateIcon(object, highlight, source)
{
	object.src = "/images/" + source + (highlight ? "" : "_pale") + ".png";
}

//
// Used to autodetect js support for squirrelmail login
//
function squirrelmail_loginpage_onload()
{
	document.webmail.js_autodetect_results.value = '1';
}

function ddg_init()
{
	// initialize squirrel if we have a form named "webmail"
//	if (document.webmail) {
//		squirrelmail_loginpage_onload();
//	}

	// If an element has id="focushere" it gets focus.
	if (document.getElementById("focushere")) {
		document.getElementById("focushere").focus()
	}
}
