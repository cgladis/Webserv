#!/usr/bin/env python3
import cgi
import html
import http.cookies
import os
import sys

import time
# time.sleep(5)

form = cgi.FieldStorage()
first_name = form.getvalue("FIRST_NAME", '')
second_name = form.getvalue("SECOND_NAME", '')
# first_name = html.escape(first_name)
# second_name = html.escape(second_name)
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))

'TO HEADER'

if os.environ.get("REQUEST_METHOD") == 'POST':
    print('Set-cookie: FIRST_NAME="{}"'.format(first_name))
    print('Set-cookie: SECOND_NAME="{}"'.format(second_name))
print("Content-type: text/html")
print()

'TO BODY'

print("""
<html lang="en">
<head>
    <link rel="shortcut icon" href="../images/ico_1.png">
    <link rel="stylesheet" href="../css/cgi_main.css">
    <link href="https://fonts.googleapis.com/css2?family=Raleway:wght@100;300;400;500;600;700;800&display=swap" rel="stylesheet">
    <meta charset="UTF-8">
</head>""")

# BODY
print('<body>')
print("""
    <div class="content">
        <header class="tab">
            <button class="tablinks" onclick="openTab(event, 'Environment')">Environment</button>
            <button class="tablinks" onclick="openTab(event, 'ARGV')">ARGV</button>
            <button class="tablinks" onclick="openTab(event, 'Cookies')">Cookies</button>
            <button class="tablinks" onclick="openTab(event, 'TestPOST')" id="defaultOpen">Test POST request</button>
        </header>
""")

print("""
        
""")

print("""
        <div id = "Environment" class="tabcontent">
""")
for param in os.environ.keys():
    print("""
            <p><b>{}</b>: {}<p>
            """.format(param, os.environ[param]))
print("""
        </div>
""")


print("""
        <div id = "ARGV" class="tabcontent">
""")
for line in sys.argv:
    print("""
            <p>{}</p>
    """.format(line))
print("""
        </div>
""")

print("""
        <div id = "Cookies" class="tabcontent">
""")
for k, v in cookie.items():
    print("""
            <p><b>{}</b>: {}</p>
    """.format(k, v.value))
print("""
        </div>
""")


cookie_first_name = cookie.get('FIRST_NAME').value if cookie.get('FIRST_NAME') is not None else ""
cookie_second_name = cookie.get('SECOND_NAME').value if cookie.get('SECOND_NAME') is not None else ""
print("""
        <div id = "TestPOST" class="tabcontent">
""")
if os.environ.get("REQUEST_METHOD") == 'GET':
    print("""
        <div class="form">
            <form action="cgi_main.py" method="post">
                <div>
                    <label for="FIRST_NAME">First name:</label>
                    <input type="text" name="FIRST_NAME" value={}>
                </div>
                <div>
                    <label for="SECOND_NAME">Second name:</label>
                    <input type="text" name="SECOND_NAME" value={}>
                </div>
                <p>
                    <button type="submit">Send</button>
                </p>
            </form>""".format(cookie_first_name, cookie_second_name))
elif os.environ.get("REQUEST_METHOD") == 'POST':

    print("""
            <br><font size=+5>Thank you for your answers</font><br>
            <p><b>{:<20}</b>: {}</p>
            <p><b>{:<20}</b>: {}</p>
            """.format("Your first name", first_name, "Your second name", second_name))
    print("""
            <form action="cgi_main.py" method="get">
                <div>
                    <button type="submit">Try again</button>
                </div>
            </form>""")

else:
    print('<br><font size=+5>I don\'t know this method', os.environ.get("REQUEST_METHOD"), '</font><br>')
    print("""
            <form action="" method="get">
                <div>
                    <button type="submit">Try again</button>
                </div>
            </form>""")


print("""
        </div>
        </div>
        
        <script>
            function openTab(evt, tabName) {
                var i, tabcontent, tablinks;
                tabcontent = document.getElementsByClassName("tabcontent");
                for (i = 0; i < tabcontent.length; i++) {
                    tabcontent[i].style.display = "none";
                }
                tablinks = document.getElementsByClassName("tablinks");
                for (i = 0; i < tablinks.length; i++) {
                    tablinks[i].className = tablinks[i].className.replace(" active", "");
                }
                document.getElementById(tabName).style.display = "block";
                evt.currentTarget.className += " active";
            }
            
            // Get the element with id="defaultOpen" and click on it
            document.getElementById("defaultOpen").click();
            
        </script>
        
    </body>
</html>
""")
