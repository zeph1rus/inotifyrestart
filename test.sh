#!/bin/sh
rm /tmp/foo
mkdir /tmp/a
mkdir /tmp/b
ln -sf /tmp/b /tmp/foo
./inrestart -p /tmp -f foo -s danted.service&
sleep 5
rm /tmp/foo; ln -sf /tmp/b foo
read -rsp $'Waiting for actions, press any key to end early...\n' -n1 key
pgrep -f inrestart | xargs kill -9
