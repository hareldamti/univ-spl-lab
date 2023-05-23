echo "Task 3a:"
readelf -s files/offensive | grep main
readelf -S files/offensive | grep .text -A 1
addr=$((0x0804841d - 0x08048320 + 0x000320))
printf "%x\n" $addr