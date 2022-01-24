#!/usr/bin/env python3
import cgi
import html
import os

form = cgi.FieldStorage()
first_name = form.getvalue("FIRST_NAME", 'empty')
second_name = form.getvalue("SECOND_NAME", 'empty')
first_name = html.escape(first_name)
second_name = html.escape(second_name)

print("Content-type: text/html")
print()
print('<html lang="en">')
print('<head>')
print('<link rel="shortcut icon" href="images/ico_for_py.png">')
print('  <style>')
print('body {')
# print('background: #202124 url(../images/bg_1.jpeg); /* Цвет фона и путь к файлу */')
# print('color: #fff; /* Цвет текста */')
print('}')
print('</style>')
print('</head>')
print()

print('<font size=+10>Environment</font><br>')

for param in os.environ.keys():
    print("<b>%20s</b>: %s<br>" % (param, os.environ[param]))

if os.environ.get("REQUEST_METHOD") == 'GET':
    print("""
            <form action="cgi_main.py" method="post">
                <p>
                    <label for="FIRST_NAME">First name:</label>
                    <input type="text" name="FIRST_NAME">
                    <label for="SECOND_NAME">Second name:</label>
                    <input type="text" name="SECOND_NAME">
                </p>
                <p>
                    <input type="submit">
                </p>
            </form>""")
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

