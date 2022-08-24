programs=("base" "date" "file_a" "file_b" "file" "macros" "file_folder/file_c")

for program in "${programs[@]}"; do
    strace -f -e 'trace=!write' -o ${program}.trace gcc -E ${program}.c -o ${program}.i
done

for program in "${programs[@]}"; do
    diff <(awk '{$1 = ""; print $0}' ${program}.trace | sed 's/0x[0-9a-f]\+/0x/g') \
     <(awk '{$1 = ""; print $0}' base.trace | sed 's/0x[0-9a-f]\+/0x/g') > ${program}.diff
done
