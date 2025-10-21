build:
	gcc -o ./disk main.c
# 	rm -rf ./disk.img

# 	dd if=/dev/zero of=fat16.img bs=512 count=65536
# 	mkfs.fat -F 16 -n "BootVol" ./fat16.img

# 	dd if=/dev/zero of=fat32.img bs=512 count=131720
# 	mkfs.fat -F 32 -n "Data" ./fat32.img

# 	./disk --create --count 128 --bs 1048576 --output ./disk.img
# 	./disk --open ./disk.img --data ./fat16.img --part boot fat16 -bs 512 -st 1 -et 65536
# 	./disk --open ./disk.img --data ./fat32.img --part fat32 -bs 512 -st 65537 -et 262144

clean:
	rm -rf ./disk