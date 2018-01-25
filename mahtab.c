#include <linux/init.h>           
#include <linux/module.h>         
#include <linux/device.h>         
#include <linux/kernel.h>         
#include <linux/fs.h>             
#include <linux/uaccess.h>          
#define  DEVICE_NAME "oslab"   
#define  CLASS_NAME  "ebb"        

MODULE_LICENSE("GPL");            
MODULE_AUTHOR("Derek Molloy");    
MODULE_DESCRIPTION("A simple Linux char driver"); 
MODULE_VERSION("0.1");           

static int    majorNumber;                 
static char   message[256] = {0};           
static short  size_of_message;              
static int    numberOpens = 0;              
static struct class*  ebbcharClass  = NULL; 
static struct device* ebbcharDevice = NULL; 

// The prototype functions for the character driver
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};
//after installing module this initialization function calls
static int __init ebbchar_init(void){
   printk(KERN_INFO "Initializing the Misc Module LKM\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "Misc Module failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "registered correctly with major number %d\n", majorNumber);

   // Register the device class
   ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ebbcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(ebbcharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "device class registered correctly\n");

   // Register the device driver
   ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(ebbcharDevice)){               // Clean up if there is an error
      class_destroy(ebbcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(ebbcharDevice);
   }
   printk(KERN_INFO "device class created correctly\n"); // Made it! device was initialized
   return 0;
}

//when we remove the module from kernel
static void __exit ebbchar_exit(void){
   device_destroy(ebbcharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(ebbcharClass);                          // unregister the device class
   class_destroy(ebbcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "Device has been opened %d time(s)\n", numberOpens);
   return 0;
}
// for read device
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   error_count = copy_to_user(buffer, message, size_of_message);
   if (error_count==0){           
      printk(KERN_INFO "mahtab farrokh :D \n");
      return 1 ;
   }
   else {
      printk(KERN_INFO "Failed to send %d characters to the user\n", error_count);
      return -EFAULT;            
   }
}
// for write device
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%zu letters)", buffer, len);   
   size_of_message = strlen(message);  
   if (strncmp(message ,"mahtab" , len)== 0 && len == 6){             
   	printk(KERN_INFO "Received Correct input : %s \n", message);
	return 1 ; //valid
   }
   else{
	printk(KERN_INFO "Received Wrong characters from the user\n");
	return 0 ; //invalid
   }
}
// the device release function that is called whenever the device is closed/released by the userspace program
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Device successfully closed\n");
   return 0;
}
module_init(ebbchar_init);
module_exit(ebbchar_exit);
