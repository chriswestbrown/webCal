/*
This Javascript generates the Navbar to my website.  Include it like this:

<div class="navbar" id="NavBar"><script type="text/javascript" src="navbar.js"></script></div>

Note that the script/src changes if you're in subdirectories.  The tricky part is that
you need to the get the relative paths right.  It does this by grabbing the value of
the script/src attribute to determine the relative path from the including document to
navbar.js, and it prepends that to the relative path links in the generated HTML below.

define variable noCSDepartmentBadge to hide the CS department badge.

 */
(function(){


    var noCSDepartmentBadge = 1; 

function pad2(n) { while(n.legnth < 2) { n = '0' + String(n); } return n; }

// Find out where we are in the file hierarchy based on the navbar.js addres
var scriptfile = document.getElementById('NavBar').innerHTML;
var baseMatch = scriptfile.match(/src\s*=\s*\"(.*)navbar\.js\"/);
var base = baseMatch[1];
var depth = base.split("../").length - 1;

var href = document.location.href.split("?")[0].split("/");
var n = href.length;
var path = []; for(var i = 0; i < depth; ++i) { path[i] = href[n - 1 - depth + i]; }
var filename = href[n-1];

var titleMatch = document.title.match(/SI413:\s*(.*)/);
var title = titleMatch[1];

var home = base + "index.html";

/*<img class="regicon" src="' + base + 'img/cal.png" onclick="document.location=\'' + base */
/*<img class="regicon" src="' + base + 'img/home.png" onclick="document.location=\'' + base 
  + 'index.html\'" title="SI413 Home"> \
<img class="regicon" src="' + base + 'img/resources.png" onclick="document.location=\'' + base 
  + 'resources/index.html\'" title="SI413 Course Resources">';
*/
var buttonSource = '\
<a href="' + base + 'index.html' + '"><span class="newButtonSmall">&#x1F3E0;</span></a> \
<a href="' + base + 'calendar.html' + '"><span class="newButtonSmall">C</span></a> \
<a href="' + base + 'resources/index.html' + '"><span class="newButtonSmall">R</span></a>';

/**********************************************************************
 favicon
***********************************************************************/
    var fi = document.createElement("link");
    fi.rel="SHORTCUT ICON";
    fi.href= base + "img/cs-logo-16x16.png";
    document.head.appendChild(fi);
    

/**********************************************************************
 Lecture navbars
***********************************************************************/
var lastLecture = 40;
if (path[0] == "lec" && path.length == 2 
    && path[1].match(/l(\d\d)/) && (filename == "lec.html" || filename == "inst.html"))
{
  var si110Home = base + "index.html";
  var num = Number(path[1].match(/l(\d\d)/)[1]); 
  var numM1 = String(num - 1);
  var numP1 = String(num + 1);
  numM1 = numM1.length == 1 ? '0' + numM1 : numM1;
  numP1 = numP1.length == 1 ? '0' + numP1 : numP1;

  // Set buttons
  var bclassL = 'lmbutton';
  var bclassR = 'lmbutton';
  var bocL = 'onclick= \'document.location="../l' + numM1 + '/' + filename + '"\'';
  var bocR = 'onclick= \'document.location="../l' + numP1 + '/' + filename + '"\'';
  if (num == 1)           { bclassL = 'lmbuttonDeactivated'; bocL = ''; }
  if (num == lastLecture) { bclassR = 'lmbuttonDeactivated'; bocR = ''; }

  // Set course section
    courseSection = 
	'/<span>Lecture ' + num + '</span>';
    
  var nbsource = '\
<h1><div class="navpath">\
<span class="pathBase">/\
<span style="cursor: pointer;" onclick=\'document.location="' + home + '";\'\>\
SI413</span>'
+ courseSection +
'/</span>' + title + '</div>\
      <img class="' + bclassL + '" src="' + base + 'img/larr.png" ' + bocL + '> \
      <img class="' + bclassR + '" src="' + base + 'img/rarr.png" ' + bocR + '> \
' + buttonSource;
  
    if( typeof(noCSDepartmentBadge) == "undefined" ) {
	nbsource = nbsource + '\
<img style="float: right; width: 10em;" src="' + base + 'img/CS-Logo-color-medium.png">\
</h1>';  
    }
    document.getElementById('NavBar').innerHTML = nbsource;
}
else
{
  var k = path.length;
  if (filename == 'index.html' && k != 0) k = k-1;

  var pd = "";
  pd += '<h1><div class="navpath"><span class="pathBase">';
  var tmp = base;
  path[-1] = 'SI413';
  for(var i = -1; i < k; ++i) 
  {
    var name = path[i];
    if (i != -1) { tmp += name + '/'; }   
    var URL = tmp + 'index.html';
    pd += '/<span style="cursor: pointer;" onclick=\'document.location="' + URL + '";\'\>' + name + '</span>' ;
  }
  
  // Exceptions to title-as-name
  var titleMOD = title;
  if (k == -1) titleMOD = "SI413";
  
  pd += '</span>/' + titleMOD + '</span>';
  pd += '</div>\
      <img class="lmbuttonInvisible" src="' + base + 'img/larr.png"> \
      <img class="lmbuttonInvisible" src="' + base + 'img/rarr.png"> ' + buttonSource;
  
  if (typeof(noCSDepartmentBadge) == "undefined")
    pd += '<img style="float: right; width: 10em;" src="' + base + 'img/CS-Logo-color-medium.png">';
  pd += '</h1>';
  document.getElementById('NavBar').innerHTML = pd;
}
})();
