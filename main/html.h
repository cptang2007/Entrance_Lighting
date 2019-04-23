#ifndef HTML_H_
#define HTML_H_

char html_data[5][300];

const static char http_header[] =
		"HTTP/1.1 200 OK\r\n"
		"Content-type: text/html\r\n\r\n";

const static char http_header2[] =
		"HTTP/1.1 200 OK\r\n"
		"Content-type: text/html\r\n"
		"Set-Cookie: __Secure-ID=123; Secure\r\n\r\n";

const static char json_header[] =
		"HTTP/1.1 200 OK\r\n"
		"Content-type: application/json; charset=utf-8\r\n\r\n";

const static char index_html_top[5400];

const static char index_html_top1[] =
		"<!DOCTYPE html>\r\n"
		"<html>\r\n"
		"<head>\r\n"
		"<meta name=\"viewport\" content=\"width=device-width, minimum-scale=1.0, initial-scale=1, user-scalable=yes\" charset=\"UTF-8\">\r\n"
		"<style>\r\n"
			"\tH1					{ font-size: large; margin-left: 0.18em;}\r\n"
			"\thtml 				{ height:100%}\r\n"
			"\tdiv.header 			{ display: inline-block; width: 100%; padding-bottom: 0.18em; padding-top: 0.18em; text-align: left; background-color: #3f51b5; color: #fff;  box-shadow: 0px 0.18em 0.375em rgba(0, 0, 0, 0.2); }\r\n"
			"\tbody 				{ font-family: 'Roboto', sans-serif; font-size: 1em; margin: 0; padding: 0em; background-color: #424242; color: #FFFFFF;  height: 100%; }\r\n"
			"\tdiv.wrapper			{ width: 100%; color: #ffffff; text-align: center; min-height: 100%; position: relative }\r\n"
			"\tdiv.content  		{ display: inline-block; text-align: left; padding: 0.3125em; font-size: small; margin-left:auto; margin-right: auto; }\r\n"
			"\tdiv.content2  		{ display: inline-block; text-align: center; padding: 0.3125em; font-size: small; margin-left:auto; margin-right: auto; }\r\n"
			"\tdiv.heading  		{ display: inline-block; text-align: center; padding: 0.3125em; color: #ff0000; font-size: medium; margin-left:auto; margin-right: auto; }\r\n"
			"\tdiv.title	  		{ margin-left: 1em; float: left; }\r\n"
			"\tdiv.ver		  		{ margin-top: 1.4em;  margin-right: 1em; float: right; font-size: 0.75em; }\r\n"
			"\tinput				{ padding-left: 0.3125em; padding-right: 0.3125em; }\r\n"
			"\tinput[type=number]::-webkit-outer-spin-button, input[type=number]::-webkit-inner-spin-button { -webkit-appearance: none; margin: 0; }\r\n"	//disable the number input spin box
			"\tinput[type=number]   { -moz-appearance:textfield;  width: 6em;}\r\n"																			//disable the number input spin box
			"\tiframe				{ visibility: hidden;}\r\n"
			"\tfooter				{ box-sizing: border-box; position: absolute; bottom: 0; width: 100%; padding-bottom:0.625em; }\r\n"
			"\tbutton				{ width: 13em; padding-left: 0.125em; padding-right: 0.125em; padding-top: 0.125em; padding-bottom: 0.125em; margin-top: 1em; border: none; border-radius: 0.0625em; box-shadow: 0.18em 0.25em 0.125em 0.0625em rgba(0,0,0,0.4); background-color: #CDCDCD; }\r\n"
			"\tbutton:active    	{ box-shadow: 0.18em 0.25em 0.125em 0.0625em rgba(0,0,0,0.6); transform: translateY(0.125em); }\r\n"
			"\tdiv.table 			{ display: table; }\r\n"
			"\tdiv.tr 				{ display: table-row; }\r\n"
			"\tdiv.tdRowHead		{ display: table-cell; padding: 0.3125em; }\r\n"
			"\tdiv.td 				{ display: table-cell; text-align: center; padding: 0.125em; padding-top: 0.5em; margin-left:auto; margin-right: auto; }\r\n"
			"\t.anim				{ opacity: 0; animation-name: FadeOut;  animation-duration: 4s; }\r\n"
			"\t@keyframes FadeOut 	{ 0% {opacity: 1;} 66% {opacity: 1;} 100% {opacity: 0;} }"
		"</style>\r\n"
		"<title>ESP32 MCU - Entrance_Lighting</title>\r\n"
		"<script>\r\n"
			"\tvar IntervalId;\r\n\r\n"

			"\tfunction readSensor() {\r\n"
				"\t\tvar xhttp = new XMLHttpRequest();\r\n"
				"\t\txhttp.onreadystatechange = function() {\r\n"
					"\t\t\tif (xhttp.readyState == 4 && xhttp.status == 200) {\r\n"
						"\t\t\t\tvar reading = JSON.parse(xhttp.responseText);\r\n"
						"\t\t\t\tdocument.getElementById(\"PR\").innerHTML = reading.PR;\r\n"
						"\t\t\t\tdocument.getElementById(\"IR_Long1\").innerHTML = reading.IR_Long1;\r\n"
						"\t\t\t\tdocument.getElementById(\"IR_Long2\").innerHTML = reading.IR_Long2;\r\n"
						"\t\t\t\tdocument.getElementById(\"IR_Short\").innerHTML = reading.IR_Short;\r\n"
				    "\t\t\t}\r\n"
				"\t\t};\r\n"
				"\t\txhttp.open(\"GET\", \"sensorReading_JSON\", true);\r\n"
				"\t\txhttp.send();\r\n"
			"\t}\r\n\r\n"

			"\tfunction updateTrigLevel() {\r\n"
				"\t\tvar PR_Trig = document.getElementsByName(\"PR_Trig\")[0].value;\r\n"
				"\t\tvar IR_Long1_Trig = document.getElementsByName(\"IR_Long1_Trig\")[0].value;\r\n"
				"\t\tvar IR_Long2_Trig = document.getElementsByName(\"IR_Long2_Trig\")[0].value;\r\n"
				"\t\tvar IR_Short_Trig = document.getElementsByName(\"IR_Short_Trig\")[0].value;\r\n"
				"\t\tvar CountDown = document.getElementsByName(\"CountDown\")[0].value;\r\n\r\n"
				"\t\tvar params =\"PR_Trig=\"+PR_Trig+\"&IR_Long1_Trig=\"+IR_Long1_Trig+\"&IR_Long2_Trig=\"+IR_Long2_Trig+\"&IR_Short_Trig=\"+IR_Short_Trig+\"&CountDown=\"+CountDown;\r\n"
				//"\t\talert(params);\r\n"

				"\t\tvar xhr = new XMLHttpRequest();\r\n"
				"\t\txhr.onreadystatechange = function() {//Call a function when the state changes.\r\n"
	    			"\t\t\tif(xhr.readyState == 4 && xhr.status == 200) {\r\n"
						"\t\t\t\tvar updateResponse = JSON.parse(xhr.responseText);\r\n\r\n"

						"\t\t\t\t//replace the element and apply effect again or animation cant restart;\r\n"
						"\t\t\t\tvar ele = document.getElementById(\"upload_result\");\r\n"
						"\t\t\t\tvar newele = ele.cloneNode(true);\r\n"
						"\t\t\t\tele.parentNode.replaceChild(newele, ele);\r\n\r\n"

						"\t\t\t\tvar ele = document.getElementById(\"upload_result\");\r\n"
						"\t\t\t\tele.classList.remove(\"anim\");\r\n"
						"\t\t\t\tele.innerHTML = updateResponse.state;\r\n"
						"\t\t\t\tele.classList.add(\"anim\");\r\n"

	    			"\t\t\t}\r\n"
				"\t\t}\r\n"
				"\t\txhr.open(\"POST\", \"SETUP\", true);\r\n"
				"\t\txhr.send(params);\r\n"

			"\t}\r\n\r\n"

			"\tfunction pollingControl() {\r\n"
				"\t\tvar btnText = document.getElementById(\"pollingButton\").textContent;\r\n\r\n"
				"\t\tif (btnText==\"Sensor Polling\") {\r\n"
					"\t\t\tIntervalId = setInterval(readSensor, 1000);\r\n"
					"\t\t\tdocument.getElementById(\"pollingButton\").textContent = \"Cancel Polling\";\r\n"
				"\t\t}\r\n"
				"\t\tif (btnText==\"Cancel Polling\") {\r\n"
					"\t\t\tclearInterval(IntervalId);\r\n"
					"\t\t\tdocument.getElementById(\"pollingButton\").textContent = \"Sensor Polling\";\r\n"
				"\t\t}\r\n"
			"\t}\r\n\r\n"

		"</script>\r\n"
		"</head>\r\n"
		"<body>\r\n"
		"<div class=\"wrapper\">\r\n"
		"\t<div class=\"header\">\r\n"
		"\t\t<div class=\"title\"><h1>ESP32 MCU - Entrance_Lighting</h1></div>\r\n"
		"\t\t<div class=\"ver\">Ver: ";

const static char index_html_top2[] =
		"</div>\r\n"
		"\t</div>\r\n"
		"<br><br>\r\n";

const static char index_html_down[] =

		"<footer>\r\n"
		"&copy; cptang.mees@gmail.com\r\n"
		"</footer>\r\n"
		"</DIV>\r\n"
		"</body>\r\n"
		"</html>\r\n";

const static char setting_html_top[] =
		"<div class=\"heading\">\r\n"
		"Parameters Setting\r\n"
		"</div>\r\n"
		"<BR><DIV class=\"content\">\r\n"
		"<FORM onsubmit=\"updateTrigLevel()\" target=\"dummyframe\">"
		"<div class=\"table\">\r\n"
		"<div class=\"tr\">\r\n"
		"\t<div class=\"td\"></div>\r\n"
		"\t<div class=\"td\" style=\"width: 9em;\">Trigger Level</div>\r\n"
		"\t<div class=\"td\" style=\"width: 9em;\">Sensor Reading</div>\r\n"
		"</div>\r\n";

const static char setting_html_down[] =
		"<div class=\"tr\">\r\n"
			"\t<div class=\"td\"></div>\r\n"
			"\t<div class=\"td\" style=\"padding-top: 0.625em;\"><button type=\"submit\" style=\"width:7em;\">Update</button></div>\r\n"
			"\t<div class=\"td\" style=\"padding-top: 0.625em;\"><button type=\"button\" style=\"width:7em;\" onclick=\"pollingControl()\" id=\"pollingButton\">Sensor Polling</button></div>\r\n"
		"</div>\r\n"
		"</div>\r\n"
		"</FORM>\r\n"
		"</DIV>\r\n"

		"<br><br><br><div id=\"upload_result\" style=\"color:#4CAF50;\"></div>"
		"<iframe width=\"0\" height=\"0\" border=\"0\" name=\"dummyframe\" id=\"dummyframe\"></iframe>";	//Dummy iframe used to redirect the form action
		//"<button style=\"width:80px;\" onclick=\"updateTrigLevel()\">Update Trig Levels</button>\r\n"
		//"<button style=\"width:80px;\" onclick=\"readSensorLoop()\">Sensor Reading</button>\r\n";

const static char http_nf[] =
		"HTTP/1.1 404 Not Found\r\n"
		"Content-type: text/html\r\n\r\n"
		"<html>\r\n"
		"<head><title>404 Not Found</title></head>\r\n"
		"<body bgcolor=\"white\">\r\n"
		"<center><h1>404 Not Found</h1></center>\r\n"
		"</body>\r\n"
		"</html>\r\n";

const static char ota_feedback_html[] =
		"<div class=\"heading\">\r\n"
		"<br>\r\n"
		"<br>OTA Image received.\r\n"
		"<br>\r\n"
		"<br>The MCU will restart auotmatically.\r\n"
		"</div>\r\n";

char ota_html[350];

const static char ota_html_top[] =
		"<div class=\"heading\">"
		"OTA Image Upload"
		"</div>"
		"<form method=\"POST\" action=\"http://";

const static char ota_html_bottom[] =
		"\" enctype=\"multipart/form-data\">\r\n"
		"<div class=\"content2\">\r\n"
		"<br><input name=\"filename\" type=\"file\" size=\"10\"><input type=\"submit\" value=\"Upload\" size=\"10\">\r\n"
		"</div>\r\n"
		"</form>\r\n";

const static char index_html[] =
		"<div class=\"content\">\r\n"
		"<br>\r\n"
		"<br><button onclick=\"window.location=\'SETUP\';\">Parameters Setting</button>\r\n"
		"<br>\r\n"
		"<br><button onclick=\"window.location=\'OTA\';\">OTA Firmware Update</button>\r\n"
		"</div>\r\n";

const static char http_basic_authentication[] =
		"HTTP/1.1 401 Access Denied\r\n"
		"WWW-Authenticate: Basic realm=\"ESP32 Server Login\"\r\n"
		"Content-Length: 0\r\n\r\n";

const char t1[]=	"<div class=\"tr\">\r\n"
					"\t<div class=\"tdRowHead\">";
const char t2[]=	"</div>\r\n"
					"\t<div class=\"td\" style=\"text-align:center;\"><input type=\"number\" name=\"";
const char t3[]=	"\" value=\"";

const char t_a[]=	"\" min=\"0\" max=\"4095\"></div>\r\n"
					"\t<div class=\"td\" style=\"color:#4CAF50;\" id=\"PR\"></div>\r\n"
					"</div>\r\n";
const char t_b[]=	"\" min=\"0\" max=\"4095\"></div>\r\n"
					"\t<div class=\"td\" style=\"color:#4CAF50;\" id=\"IR_Long1\"></div>\r\n"
					"</div>\r\n";
const char t_c[]=	"\" min=\"0\" max=\"4095\"></div>\r\n"
					"\t<div class=\"td\" style=\"color:#4CAF50;\" id=\"IR_Long2\"></div>\r\n"
					"</div>\r\n";
const char t_d[]=	"\" min=\"0\" max=\"4095\"></div>\r\n"
					"\t<div class=\"td\" style=\"color:#4CAF50;\" id=\"IR_Short\"></div>\r\n"
					"</div>\r\n";
const char t_e[]=	"\" min=\"1000\" max=\"600000\"></div>\r\n"
					"\t<div class=\"td\"></div>\r\n"
					"</div>\r\n";

const char CRLF[]="\r\n";
const char JSON_message2[]="{ \"state\":\"Setting Updated\" }";
#endif
