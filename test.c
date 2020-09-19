#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>

struct bitfield{
	uint8_t hi;
	uint8_t lo;
	char *name;
}bitfield;

struct msr{
	uint32_t addr;
	char *name;
	struct bitfield bitfield[];
};

static struct msr foo[2]={
	{ 0x610, "MSR_PKG_POWER_LIMIT", .bitfield= 
						{  63, 63, "lock" },
						{  62, 56, "reserved" },
						{  55, 49, "window2" },
						{  48, 48, "clamp2" },
						{  47, 47, "enable2" },
						{  46, 32, "limit2" },
						{  31, 24, "reserved" },
						{  23, 17, "window1" },
						{  16, 16, "clamp1" },
						{  15, 15, "enable1" },
						{  14,  0, "limit1" }
	},
	{ 0x611, "MSR_PKG_ENERGY_STATUS", .bitfield= 
						{  63, 32, "reserved" },
						{  31,  0, "joules" }
	}
};


int main(){
	fprintf( stdout, "%s\n", foo[1].bitfield[1].name );		
	return 0;
}
													     


