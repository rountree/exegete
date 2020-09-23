#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define MAX_BITFIELDS_PER_MSR ((uint8_t)(32))

typedef struct bitfield{
	const uint8_t h;
	const uint8_t l;
	const char *const name;
}bitfield_t;

typedef struct msr{
	const char *const name;
	const uint64_t address;
	const bitfield_t bitfield[MAX_BITFIELDS_PER_MSR];	// Because we have memory to burn.
}msr_t;

msr_t msr_list[2] = { 
	{ .name="a1", .address=1, .bitfield={
						   { .h=1, .l=1, .name="b1"}, 
						   { .h=2, .l=2, .name="b2"}
					   }
	},
	{ .name="a2", .address=1, .bitfield={
						   { .h=10, .l=10, .name="b3"}, 
						   { .h=20, .l=20, .name="b4"}
					   }
	}
};

int main(){
	fprintf( stdout, "0x%" PRIx8 "\n", msr_list[1].bitfield[1].h );
	return 0;
}

