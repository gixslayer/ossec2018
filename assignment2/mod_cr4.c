#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ciske Harsema");
MODULE_DESCRIPTION("A simple module to read the CR4 register");

// Output on my Kali: Value of CR4 = 606f0

static int __init cr4_init(void) {
	unsigned long long result;

	__asm__("movq %%cr4, %%rax\n" : "=a"(result));

	printk(KERN_INFO "Value of CR4 = %llx\n", result);

	return 0;
}

static void __exit cr4_exit(void) {}

module_init(cr4_init);
module_exit(cr4_exit);
