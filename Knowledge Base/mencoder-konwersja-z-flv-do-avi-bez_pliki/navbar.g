<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html dir="ltr"><head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8"><link href="navbar_data/4034018350-navbar4_bundle.css" rel="stylesheet" type="text/css">
<!-- base --></head><body class="null lang_pl"><script type="text/javascript" src="navbar_data/3625575137-cookies.js"></script>
<script type="text/javascript" src="navbar_data/4095335807-common.js"></script>
<script type="text/javascript" src="navbar_data/107652916-dom.js"></script>
<script type="text/javascript">
        var ID = "2068752410649084043";
        var FLAG_COOKIE_NAME = 'flaggedBlog';
        var MAX_FLAGGED_BLOGS = 10;
        var FLAG_BLOG_URL = "http://www.blogger.com" +
                            "/flag-blog.g?nav=" +
                            "4" +
                            "&toFlag=" + ID;
        var UNFLAG_BLOG_URL = "http://www.blogger.com" +
                            "/unflag-blog.g?nav=" +
                            "4" +
                            "&toFlag=" + ID;

        var ncHasFlagged = false;
        var servletTarget = new Image();

       
       function hasFlagged() {
         if (getCookie(FLAG_COOKIE_NAME)) {
           var bloglist = getCookie(FLAG_COOKIE_NAME);
           var blogarray = bloglist.split(",")
           if (FindInArray(blogarray, ID) > 0) {
             return true;
           }
         }
         return ncHasFlagged;
       }
       

       function toggleFlag() {
         var date = new Date();
         var flagBtn = document.getElementById('b-flag-this');

         if (hasFlagged()) {
           removeBlogFromFlagCookie();
           servletTarget.src = UNFLAG_BLOG_URL + '&d=' + date.getTime();

           RemoveClass(flagBtn, 'flagged');
           ncHasFlagged = false;
         } else {
           setBloggerFlagCookie();
           servletTarget.src = FLAG_BLOG_URL + '&d=' + date.getTime();

           AddClass(flagBtn, 'flagged');
           ncHasFlagged = true;
         }

         refreshDrop();
       }

       
       function showDrop() {
         var overlap = 5;
         var dropdown_position =
           GetPageOffsetRight(document.getElementById('b-flag-this')) - overlap + "px";

         document.getElementById('unflagi').style.display = 'none';
         document.getElementById('flagi').style.display = 'none';

         if (!hasFlagged()) {
           document.getElementById('flagi').style.display = 'inline';
           document.getElementById('flagi').style.left = dropdown_position;
           showElement(document.getElementById('flagi'));
         } else {
           document.getElementById('unflagi').style.display = 'inline';
           document.getElementById('unflagi').style.left = dropdown_position;
           showElement(document.getElementById('unflagi'));
         }
       }

       
       function hideDrop() {
         hideElement(document.getElementById('flagi'));
         hideElement(document.getElementById('unflagi'));
       }

       
       function refreshDrop() {
         hideDrop();
         showDrop();
       }

       
       function setBloggerFlagCookie() {
         var bloglist = ""
         if (getCookie(FLAG_COOKIE_NAME)) {
           bloglist = getCookie(FLAG_COOKIE_NAME)
           var blogarray = bloglist.split(",")
           if (blogarray.length >= MAX_FLAGGED_BLOGS) {
             blogarray.shift()
           }
           InsertArray(blogarray, ID)
           bloglist = blogarray.toString();
         } else {
           bloglist = ID
         }
         setCookie(FLAG_COOKIE_NAME, bloglist, null, null, '/', null)
       }

       
       function removeBlogFromFlagCookie(){
         if (getCookie(FLAG_COOKIE_NAME)) {
           var bloglist = getCookie(FLAG_COOKIE_NAME);
           var blogarray = bloglist.split(",")
           if (FindInArray(blogarray, ID) > 0) {
             DeleteArrayElement(blogarray, ID)
             bloglist = blogarray.toString();
           }
           setCookie(FLAG_COOKIE_NAME, bloglist, null, null, '/', null);
         }
       }

      </script>
<div id="flagi" style="position: absolute;" onmouseover="showDrop()" onmouseout="hideDrop();">Powiadom Bloggera o  <a style="" href="http://help.blogger.com/bin/answer.py?answer=42517" target="_blank">kontrowersyjnej zawartości</a> tej strony.</div>
<div id="unflagi" style="position: absolute;" onmouseover="showDrop()" onmouseout="hideDrop()">Ten blog został oflagowany przez Ciebie jako blog o  <a style="" href="http://help.blogger.com/bin/answer.py?answer=42517" target="_blank">kontrowersyjnej zawartości</a>.</div>
<div id="b-navbar"><a style="" href="http://www.blogger.com/" id="b-logo" tabindex="1" title="Przejdź do witryny Blogger.com"><div id="navbar-logo"><span>Blogger</span></div></a>
<div id="b-sms" class="b-mobile"><a href="sms:?body=Witaj%2C%20odwied%C5%BA%20bloga%20Blog%20o%20Linux%20Ubuntu%20pod%20adresem%20http%3A%2F%2Fwww.ubucentrum.net%2F" tabindex="2">Wyślij jako SMS</a></div>
<div id="b-search"><table border="0" cellpadding="0" cellspacing="0"><tbody><tr><td class="navsearch" nowrap="nowrap" valign="middle"><form id="searchthis" action="http://www.ubucentrum.net/search" method="get" style="display: inline;"><table cellpadding="0" cellspacing="0"><tbody><tr><td valign="middle"><input id="b-query" name="q" tabindex="3" title="Przeszukaj bloga" type="text"></td>
<td class="navbutton" valign="middle"><noscript><input src="navbar_data/btn_search_this.png" id="b-searchbtn" tabindex="4" alt="Szukaj w tym blogu" title="Szukaj w tym blogu" type="image"></noscript>
<script type="text/javascript">
  document.write("\x3cdiv id\x3d\x22b-search-this\x22 class\x3d\x22btn\x22 onclick\x3d\x22document.getElementById(\x26#39;searchthis\x26#39;).submit()\x22 tabindex\x3d\x224\x22\x3e\x3cdiv\x3ePrzeszukaj bloga\x3c/div\x3e\x3c/div\x3e");
</script></td>
<td class="navbutton" valign="middle"><script type="text/javascript">
  document.write("\x3cdiv id\x3d\x22b-flag-this\x22 class\x3d\x22btn\x22 onclick\x3d\x22toggleFlag();\x22 onmouseover\x3d\x22showDrop()\x22 onmouseout\x3d\x22hideDrop()\x22 tabindex\x3d\x225\x22\x3e\x3cdiv id\x3d\x22bt-flag-body\x22\x3e\x3cimg class\x3d\x22flag\x22 alt\x3d\x22\x22 src\x3d\x22/img/blank.gif\x22\x3e\n\x3cspan class\x3d\x22flag-text\x22\x3eOflaguj bloga\x3c/span\x3e\x3c/div\x3e\n\x3cdiv id\x3d\x22bt-unflag-body\x22\x3e\x3cimg class\x3d\x22flag\x22 alt\x3d\x22\x22 src\x3d\x22/img/blank.gif\x22\x3e\n\x3cspan class\x3d\x22flag-text\x22\x3eUsu\u0144 flag\u0119 z bloga\x3c/span\x3e\x3c/div\x3e\x3c/div\x3e");
</script></td></tr></tbody></table></form></td>


<td nowrap="nowrap" valign="middle" width="100%"><a style="" href="http://www.blogger.com/next-blog?navBar=true" id="b-next" tabindex="6">Następny blog»</a></td>
<td class="navbar-right" align="right" nowrap="nowrap" valign="middle">
<a style="" href="http://www.blogger.com/signup.g" id="b-getorpost" tabindex="7">Utwórz bloga</a>
|
<a style="" href="http://www.blogger.com/" tabindex="8">Zaloguj się</a></td></tr></tbody></table>
<div id="b-search-img"></div></div></div>
<script type="text/javascript">
        function closePopup(wnd) {
          wnd.close();
        }
      </script>
<hints id="hah_hints"></hints><a style="visibility: visible; display: block;" href="http://help.blogger.com/bin/answer.py?answer=42517" target="_blank">kontrowersyjnej zawartości</a><a style="visibility: visible; display: block;" href="http://help.blogger.com/bin/answer.py?answer=42517" target="_blank">kontrowersyjnej zawartości</a><a style="visibility: visible; display: block;" href="http://www.blogger.com/" id="b-logo" tabindex="1" title="Przejdź do witryny Blogger.com"><div id="navbar-logo"><span>Blogger</span></div></a><a style="visibility: visible; display: block;" href="http://www.blogger.com/next-blog?navBar=true" id="b-next" tabindex="6">Następny blog»</a><a style="visibility: visible; display: block;" href="http://www.blogger.com/signup.g" id="b-getorpost" tabindex="7">Utwórz bloga</a><a style="visibility: visible; display: block;" href="http://www.blogger.com/" tabindex="8">Zaloguj się</a></body></html>