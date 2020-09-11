// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>

static int __init exegete_init(void)
{
	printk( KERN_INFO "Module exegete loaded successfully.\n");
	return 0;
}

static void __exit exegete_exit(void){
	printk( KERN_INFO "Module exegete shuffling off this mortal coil....\n");
}
module_init(exegete_init);
module_exit(exegete_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Barry Rountree <rountree@llnl.gov>");
