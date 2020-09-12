// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cpumask.h>	// for_each_online_cpu()
#include <linux/topology.h>	// topology_physical_package_id()


/* Topology information is scattered among:
 * 	Documentation/x86/topology.rst
 * 	Documentation/admin-guide/cputopology.rst
 *
 * 	For hyperthreading test grep smp_num_siblings in x86/kernel/cpu/common.c
 *
 *
 */

static cpumask_var_t per_package_mask;
static cpumask_var_t per_core_mask;
// static cpumask_var_t per_thread_mask; (Use cpu_online_mask.)

static int get_physical_package_count(void){

	// Stolen from drivers/idle/intel_idle.c
	int cpu, package_num, num_sockets = 1;

	for_each_online_cpu(cpu) {
		package_num = topology_physical_package_id(cpu);
		if (package_num + 1 > num_sockets) {
			num_sockets = package_num + 1;
		}
	}
	return num_sockets;
}

static u16 get_max_cores_per_package(void){
	struct cpuinfo_x86 *c = &cpu_data(0);
	return c->x86_max_cores;
}

static int __init exegete_init(void)
{
	bool enough_memory;
	int cpu, this_package_num, this_core_num, next_package=0, next_core=0;

	printk( KERN_INFO "Module exegete loaded successfully.\n");
	printk( KERN_INFO "Number of packages=%d.\n", get_physical_package_count() );
	printk( KERN_INFO "Number of cores per package=%d\n", get_max_cores_per_package() );
	printk( KERN_INFO "Number of hyperthreads per core=%d\n", smp_num_siblings);
	printk( KERN_INFO "Number of cpu ids=%d\n", nr_cpu_ids);
	printk( KERN_INFO "Number of possible cpus=%d\n", num_possible_cpus() );
	printk( KERN_INFO "Number of present cpus=%d\n", num_present_cpus() );
	printk( KERN_INFO "Number of active cpus=%d\n", num_active_cpus() );

	// Allocate memory for cpu masks.
	enough_memory = zalloc_cpumask_var( &per_package_mask, GFP_KERNEL); 
	if(!enough_memory) goto per_package_mask_err;

	enough_memory = zalloc_cpumask_var( &per_core_mask, GFP_KERNEL); 
	if(!enough_memory) goto per_core_mask_err;


	// Populate one cpu per package and per core.
	for_each_online_cpu( cpu ) {
		this_package_num = topology_physical_package_id(cpu);
		if( this_package_num == next_package ){
			next_package++;
			cpumask_set_cpu( cpu, per_package_mask );
		}
		this_core_num = topology_core_id( cpu );
		if( this_core_num == next_core ){
			next_core++;
			cpumask_set_cpu( cpu, per_core_mask );
		}
	}

	printk( KERN_INFO "Weight of per_package_mask=%d\n", cpumask_weight( per_package_mask ) );
	printk( KERN_INFO "Weight of per_core_mask=%d\n", cpumask_weight( per_core_mask ) );
	printk( KERN_INFO "Weight of per_thread_mask=%d\n", cpumask_weight( cpu_online_mask ) );
		
	return 0;

per_core_mask_err:
	free_cpumask_var( per_package_mask );
per_package_mask_err:
	return -ENOMEM;
}

static void __exit exegete_exit(void){
	free_cpumask_var( per_core_mask );
	free_cpumask_var( per_package_mask );
	printk( KERN_INFO "Module exegete shuffling off this mortal coil....\n");
}
module_init(exegete_init);
module_exit(exegete_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Barry Rountree <rountree@llnl.gov>");
