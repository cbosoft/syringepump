#!/usr/bin/python3

ignore_libs = ['libpango', 'libgtk', 'libgobject', 'libglib', 'libfreetype', 'libfontconfig', 'libgdk', 'libdl', 'libcairo', 'libX11', 'librsvg', 'libpixman', 'libdbus']
err_types = ['Cond', 'Leak', 'Addr8', 'Value8']

for lib in ignore_libs:
    for err in err_types:
        supp = f'{{\n  <{lib}>\n  Memcheck:{err}\n  ...\n  obj:/usr/*lib*/{lib}*\n}}'
        print(supp)
