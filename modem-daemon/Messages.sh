#! /usr/bin/env bash
$XGETTEXT `find -name \*.cpp -o -name \*.qml -o -name \*.h` -o $podir/modem-daemon_qt.pot

