echo "Task 0a:"
readelf -h files/abc | grep Entry
readelf -h files/abc | grep "Number of section"