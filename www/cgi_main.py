#!/usr/bin/env python3

import os

print('<!DOCTYPE html>')
print('<html lang="en">')
print('<head>')
print('<link rel="shortcut icon" href="../images/ico_for_py.png">')
print('  <style>')
print('body {')
print('background: #202124 url(../images/bg_1.jpeg); /* Цвет фона и путь к файлу */')
print('color: #fff; /* Цвет текста */')
print('}')
print('</style>')
print('</head>')
print()

print('<font size=+10>Environment</font><br>')

for param in os.environ.keys():
    print("<b>%20s</b>: %s<br>" % (param, os.environ[param]))