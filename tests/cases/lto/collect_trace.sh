strace -f -e 'trace=!write' -o lto.strace g++ -flto -o f1.o -c f1.cpp
strace -f -e 'trace=!write' -o nolto.strace g++ -o f1.o -c f1.cpp

diff <(awk '{$1 = ""; print $0}' nolto.strace | sed 's/0x[0-9a-f]\+/0x/g') \
     <(awk '{$1 = ""; print $0}' lto.strace | sed 's/0x[0-9a-f]\+/0x/g') > lto_strace.diff

ltrace -f -o lto.ltrace g++ -flto -o f1.o -c f1.cpp
ltrace -f -o nolto.ltrace g++ -o f1.o -c f1.cpp

diff <(awk '{$1 = ""; print $0}' nolto.ltrace | sed 's/0x[0-9a-f]\+/0x/g') \
     <(awk '{$1 = ""; print $0}' lto.ltrace | sed 's/0x[0-9a-f]\+/0x/g') > lto_ltrace.diff

