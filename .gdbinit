#set tdesc filename target.xml
set target-async on
target remote localhost:50000
set remotetimeout 5
mon arm semihosting enable
set print pretty on
set logging on
set logging file gdb_session.txt
