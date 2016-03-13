TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    compiler \
    test

test.depends = compiler
