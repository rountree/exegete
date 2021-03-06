// SPDX-License-Identifier: GPL-2.0
/* 
 * Exegete:  A better msr interface.
 *
 */
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

/*	What the /sysfs directory looks like					Structs w/ kobjects

	/sys/
		ex/								struct ex_obj
			cpu0/							struct scope_obj
				....
				0x610_MSR_PKG_POWER_LIMIT/			struct msr_obj
					address									address
					name									name
					notes									notes
					scope									scope
					value									value
					Power_Limit_1/				struct bitfield_obj
						notes								notes
						range								range
						value								value
					Power_Limit_2/
					...
				...
			per_package/
			per_core/
			per_cpu/
			batch/

*/

//
// Master msr list.
//

typedef enum {
	PACKAGE_SCOPE,
	CORE_SCOPE,
	CPU_SCOPE,	// aka thread
} scope_t;

static struct msr_master{
	u32 address;
	u64 writemask;
	scope_t scope;
};

//
// struct kobject containers that make up the sysfs directory structure.
//

struct ex_obj{
	struct kobject kobj;
	struct scope_obj *scope;
	u32 num_scopes;
};
#define to_ex_obj(x) container_of(x, struct ex_obj, kobj)


struct scope_obj{
	struct kobject kobj;
	cpumask_var_t cpumask;
	struct msr_obj *msr;
	u32 num_msrs;
};
#define to_scope_obj(x) container_of(x, struct scope_obj, kobj)

struct msr_obj{
	struct kobject kobj;
	u32 msr_idx;
	struct bitfield_obj *bitfield;
};
#define to_msr_obj(x) container_of(x, struct msr_obj, kobj)

struct bitfield_obj{
	struct kobject kobj;
	u32 bitfield_idx;
};
#define to_bitfield_obj(x) container_of(x, struct msr_obj, kobj)

//
// Functions attached to attributes
//
static ssize_t show_msr_address(struct kobject *kobj, struct kobj_attribute *attr, char *buf){ }
static ssize_t show_msr_name(struct kobject *kobj, struct kobj_attribute *attr, char *buf){ }
static ssize_t show_msr_notes(struct kobject *kobj, struct kobj_attribute *attr, char *buf){ }
static ssize_t show_msr_scope(struct kobject *kobj, struct kobj_attribute *attr, char *buf){ }
static ssize_t show_msr_value(struct kobject *kobj, struct kobj_attribute *attr, char *buf){ }
static ssize_t store_msr_value(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){ }

static ssize_t show_bits_notes(struct kobject *kobj, struct kobj_attribute *attr, char *buf){ }
static ssize_t show_bits_range(struct kobject *kobj, struct kobj_attribute *attr, char *buf){ }
static ssize_t show_bits_value(struct kobject *kobj, struct kobj_attribute *attr, char *buf){ }
static ssize_t store_bits_value(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){ }

//
// struct attribute containers that describe the files in the directory structure.
//

static struct kobj_attribute msr_address_attribute   	= __ATTR(raw, 0440, show_msr_address, NULL);
static struct kobj_attribute msr_name_attribute   	= __ATTR(raw, 0440, show_msr_name, NULL);
static struct kobj_attribute msr_notes_attribute   	= __ATTR(raw, 0440, show_msr_notes, NULL);
static struct kobj_attribute msr_scope_attribute   	= __ATTR(raw, 0440, show_msr_scope, NULL);
static struct kobj_attribute msr_value_attribute  	= __ATTR(raw, 0660, show_msr_value, store_msr_value);

static struct kobj_attribute bits_notes_attribute   	= __ATTR(raw, 0440, show_bits_notes, NULL);
static struct kobj_attribute bits_range_attribute   	= __ATTR(raw, 0440, show_bits_range, NULL);
static struct kobj_attribute bits_value_attribute  	= __ATTR(raw, 0660, show_bits_value, store_bits_value);

static struct attribute *msr_attrs[] = { 
	&msr_address_attribute.attr,
	&msr_name_attribute.attr,
	&msr_notes_attribute.attr,
	&msr_scope_attribute.attr,
	&msr_value_attribute.attr,
	NULL,
};

static struct attribute *bits_attrs[] = {
	&bits_notes_attribute.attr,
	&bits_range_attribute.attr,
	&bits_value_attribute.attr,
	NULL,
};

static struct attribute_group msr_attribute_group{
	.attrs=msr_attrs;
};

static struct attribute_group bits_attribute_group{
	.attrs=bits_attrs;
};

