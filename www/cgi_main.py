#!/usr/bin/env python3
import cgi
import html
import http.cookies
import os

form = cgi.FieldStorage()
first_name = form.getvalue("FIRST_NAME", 'empty')
second_name = form.getvalue("SECOND_NAME", 'empty')
# first_name = html.escape(first_name)
# second_name = html.escape(second_name)
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))

'TO HEADER'

if os.environ.get("REQUEST_METHOD") == 'POST':
    print("Set-cookie: FIRST_NAME={}".format(first_name))
    print("Set-cookie: SECOND_NAME={}".format(second_name))
print("Content-type: text/html")
print()

'TO BODY'

print("""
<html lang="en">
<head>
    <link rel="shortcut icon" href="../images/ico_for_py.png">
    <style>
        body {{
            background: #333333 url({}); /* Цвет фона и путь к файлу */
            color: #f5f5f7; /* Цвет текста */
        }}
    </style>
</head>""".format(''))

# print()
print('<body>')
print('<font size=+10>Environment</font><br>')

for param in os.environ.keys():
    print("<b>%20s</b>: %s<br>" % (param, os.environ[param]))

cookie_first_name = cookie.get('FIRST_NAME').value if cookie.get('FIRST_NAME') is not None else ""
cookie_second_name = cookie.get('SECOND_NAME').value if cookie.get('SECOND_NAME') is not None else ""
print('cookie_first_name {} cookie_second_name {}'.format(cookie_first_name, cookie_second_name))

if os.environ.get("REQUEST_METHOD") == 'GET':
    print("""
            <form action="cgi_main.py" method="post">
                <p>
                    <label for="FIRST_NAME">First name:</label>
                    <input type="text" name="FIRST_NAME" value={}>
                    <label for="SECOND_NAME">Second name:</label>
                    <input type="text" name="SECOND_NAME" value={}>
                </p>
                <p>
                    <input type="submit">
                </p>
            </form>""".format(cookie_first_name, cookie_second_name))
elif os.environ.get("REQUEST_METHOD") == 'POST':

    print('<font size=+5>Thank you for your answers</font><br>')
    print("<b>{:<20}</b>: {}<br>".format("Your first name", first_name))
    print("<b>{:<20}</b>: {}<br>".format("Your second name", second_name))
    print("""
            <form action="" method="get">
                <p>
                    <input type="submit">
                </p>
            </form>""")

else:
    print('<font size=+5>I don\'t know this method', os.environ.get("REQUEST_METHOD"), '</font><br>')
    print("""
            <form action="" method="get">
                <p>
                    <input type="submit">
                </p>
            </form>""")

print('</body>')
print('</html>')
