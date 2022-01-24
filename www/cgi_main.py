#!/usr/bin/env python3

import os

print('<!DOCTYPE html>')
print('<html lang="en">')
print('<head>')
print('<link rel="shortcut icon" href="../images/ico_for_py.png">')
# print('  <style>')
# print('body {')
# print('background: #202124 url(../images/bg_1.jpeg); /* Цвет фона и путь к файлу */')
# print('color: #fff; /* Цвет текста */')
# print('}')
print('</style>')
print('</head>')
print()

print('<font size=+10>Environment</font><br>')

for param in os.environ.keys():
    print("<b>%20s</b>: %s<br>" % (param, os.environ[param]))

if os.environ.get("REQUEST_METHOD") == 'GET':
    print("""
            <form action="" method="post">
                <p>
                    <label for="login">login</label>
                    <input type="text" name="login">
                </p>
                <p>
                    <label for="password">password</label>
                    <input type="text" name="password">
                </p>
                <p>
                    <input type="submit">
                </p>
            </form>""")
elif os.environ.get("REQUEST_METHOD") == 'POST':
    print('<font size=+5>Thank you for yor answers</font><br>')
    print('Your login:', os.environ.get("login"), '<br>')
    print('Your password:', os.environ.get("password"), '<br>')
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

