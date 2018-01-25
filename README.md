## misc char device driver
In this kernel module when the character deivce node is read from, my name is returned to the caller and when the character device node is written to, module checks if it matches my name and then return a correct write return value. if the value does not match my name, return the "invalid value"
you can use it by : 
make
insmod mahtab.ko
sudo ./test 
