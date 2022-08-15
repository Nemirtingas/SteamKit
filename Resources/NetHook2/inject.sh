
echo "set unwindonsignal on
call (void*)dlopen(\"$(pwd)/NetHook2.so\", 2)" | gdb -p 18758
