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

static unsigned long long raw;

static ssize_t raw_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	return sprintf(buf, "0x%llx\n", raw);
}

static ssize_t raw_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
	
	int ret;

	ret = kstrtoull(buf, 0, &raw);
	if( ret < 0 ){
		return ret;
	}

	return count;
}

static struct kobj_attribute raw_attribute = __ATTR(raw, 0664, raw_show, raw_store);

static struct attribute *attrs[] = {
	&raw_attribute.attr,
	NULL
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

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

static void warm_fuzzies(void){
	printk( KERN_INFO "Module exegete loaded successfully.\n");
	printk( KERN_INFO "Number of packages=%d.\n", get_physical_package_count() );
	printk( KERN_INFO "Number of cores per package=%d\n", get_max_cores_per_package() );
	printk( KERN_INFO "Number of hyperthreads per core=%d\n", smp_num_siblings);
	printk( KERN_INFO "Number of cpu ids=%d\n", nr_cpu_ids);
	printk( KERN_INFO "Number of possible cpus=%d\n", num_possible_cpus() );
	printk( KERN_INFO "Number of present cpus=%d\n", num_present_cpus() );
	printk( KERN_INFO "Number of active cpus=%d\n", num_active_cpus() );
	printk( KERN_INFO "Weight of per_package_mask=%d\n", cpumask_weight( per_package_mask ) );
	printk( KERN_INFO "Weight of per_core_mask=%d\n", cpumask_weight( per_core_mask ) );
	printk( KERN_INFO "Weight of per_thread_mask=%d\n", cpumask_weight( cpu_online_mask ) );
	printk( KERN_INFO "sizeof(unsigned long)=%zu\n", sizeof(unsigned long) );
	printk( KERN_INFO "sizeof(unsigned long long)=%zu\n", sizeof(unsigned long long) );
}

/* initialize_cpumasks(void)
 *
 * Allocates memory for an initializes per_package_make and per_core_mask.
 * Returns 0 on success or -ENOMEM on failure.
 */
static int initialize_cpumasks(void){

	bool enough_memory;
	int cpu, this_package_num, this_core_num, next_package=0, next_core=0;

	// Allocate memory for cpu masks.
	enough_memory = zalloc_cpumask_var( &per_package_mask, GFP_KERNEL); 
	if(!enough_memory) {
		printk( KERN_CRIT "exegete: unable to allocate sufficient memory for per_package_mask.\n" );
		goto per_package_mask_err;
	}

	enough_memory = zalloc_cpumask_var( &per_core_mask, GFP_KERNEL); 
	if(!enough_memory) {
		printk( KERN_CRIT "exegete: unable to allocate sufficient memory for per_core_mask.\n" );
		goto per_core_mask_err;
	}

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
		
	return 0;

per_core_mask_err:
	free_cpumask_var( per_package_mask );
per_package_mask_err:
	return -ENOMEM;

}

enum{
	PKG_POWER_LIMIT,
	RAPL_UNITS,
	NUM_MSRS
};

static struct kobject *exegete_kobj;			// Root for this project.
static struct kobject *msr_kobj[NUM_MSRS];	// This is going to get much larger...

static int allocate_kobjects(void){
	int rc;

	exegete_kobj = kobject_create_and_add("exegete", NULL);
	if( exegete_kobj == NULL ){ return -ENOMEM; }

	msr_kobj[PKG_POWER_LIMIT] = kobject_create_and_add("pkg_power_limit", exegete_kobj);
	if( msr_kobj[PKG_POWER_LIMIT] == NULL ){ return -ENOMEM; }
	
	msr_kobj[RAPL_UNITS] = kobject_create_and_add("rapl_units", exegete_kobj);
	if( msr_kobj[RAPL_UNITS] == NULL ){ return -ENOMEM; }

	rc = sysfs_create_group(msr_kobj[PKG_POWER_LIMIT], &attr_group);
	if( rc ){ 
		kobject_put( msr_kobj[PKG_POWER_LIMIT] ); 
		return rc;
	}
	rc = sysfs_create_group(msr_kobj[RAPL_UNITS], &attr_group);
	if( rc ){ 
		kobject_put( msr_kobj[RAPL_UNITS] ); 
		return rc;
	}

	return 0;
}

static void deallocate_kobjects(void){

	kobject_put( msr_kobj[PKG_POWER_LIMIT] );
	kobject_put( msr_kobj[RAPL_UNITS] );
	kobject_put( exegete_kobj );
}


static int __init exegete_init(void)
{
	int rc;
	rc = initialize_cpumasks();
	rc = allocate_kobjects();
	warm_fuzzies();
	return rc;
}

static void __exit exegete_exit(void){
	deallocate_kobjects();
	free_cpumask_var( per_core_mask );
	free_cpumask_var( per_package_mask );
	printk( KERN_INFO "exegete:  unloaded.\n");
}

module_init(exegete_init);
module_exit(exegete_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Barry Rountree <rountree@llnl.gov>");
