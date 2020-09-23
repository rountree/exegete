// SPDX-License-Identifier: GPL-2.0

/* Each msr has (for now) an address, a name, a scope, a write mask, and some number of bitfields.
 * Each bitfield has (for now) a high bit, a low bit, a write mask, and a name.
 * The number of bitfields can range from 0 to a couple dozen.
 */

#define MAX_BITFIELDS_PER_MSR (32)
#define MASK(H,L) (((H-L)+1)<<H)
#define ROMASK(H,L) .writemask=0x0,.h=H,.l=L
#define RWMASK(H,L) .writemask=MASK(H,L),.h=H,.l=L

typedef enum{
	THREAD_SCOPE,
	CORE_SCOPE,
	PACKAGE_SCOPE,
	NUM_SCOPES
} scope_t;

typedef struct bitfield{
	struct kobject	kobj;
	const char 	*name;
	const u64	writemask;	
	const u8	h;
	const u8	l;
} bitfield_t;

typedef struct msr{
	struct kobject		kobj;
	const char 		*name;
	const u64		writemask;
	const u32		address;
	const scope_t 		scope;
	const bitfield_t	bitfield[MAX_BITFIELDS_PER_MSR];
	const u8		nbitfields;
}msr_t;

msr_t msr_list[] = {
	{ .name="MSR_RAPL_POWER_UNIT", 		.writemask=0x0, .address=0x606, .scope=PACKAGE_SCOPE, .nbitfields=3, .bitfield={
		{.name="power_units", 		ROMASK(3,0)},
		{.name="energy_units", 		ROMASK(12,8)},
		{.name="time_units", 		ROMASK(19,16)}}},

	{ .name="MSR_PKG_POWER_LIMIT", 		.writemask=0x80ffffff00ffffff, .address=0x610, .scope=PACKAGE_SCOPE, .nbitfields=9, .bitfield={
		{.name="lock", 			.writemask=MASK(63,63), 	.h=63, 	.l=63},
		{.name="time_window_2", 	.writemask=MASK(55,49), 	.h=55, 	.l=49},
		{.name="enable_clamping_2",	.writemask=MASK(48,48),		.h=48, 	.l=48},
		{.name="enable_limit_2",	.writemask=MASK(47,47),		.h=47,	.l=47},
		{.name="power_limit_2",		.writemask=MASK(46,32),		.h=46,	.l=32},
		{.name="time_window_1", 	.writemask=MASK(23,17), 	.h=23, 	.l=17},
		{.name="enable_clamping_1",	.writemask=MASK(16,16),		.h=48, 	.l=48},
		{.name="enable_limit_1",	.writemask=MASK(15,15),		.h=47,	.l=47},
		{.name="power_limit_1",		.writemask=MASK(14, 0),		.h=46,	.l=32},

						      

};

	

