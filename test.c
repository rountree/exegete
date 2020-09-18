struct bitfield{
	char *name;
	uint8_t idx;
	uint8_t hi;
	uint8_t lo;
}bitfield;

struct msr{
	uint32_t addr;
	char *name;
	uint8_t num_bitfields;
	struct bitfield bitfield[];
};

static struct msr foo={
	{ .addr=0x610, .name="MSR_PKG_POWER_LIMIT", .num_bitfields=9, .bitfield={
		{ .name="lock", 
													     


