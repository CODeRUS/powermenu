TEMPLATE = subdirs
SUBDIRS = \
    daemon \
    gui \
    desktopfilemodel \
    $${NULL}

OTHER_FILES = \
    rpm/powermenu.spec \
    rpm/powermenu.changes.in \
    $${NULL}
