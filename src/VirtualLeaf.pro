TEMPLATE = subdirs

SUBDIRS = \
Library \   # relative paths
GUI \
Models

app.subdir = GUI
lib.subdir = Library
lib2.subdir = Models

app.depends = lib
lib2.depends = lib
