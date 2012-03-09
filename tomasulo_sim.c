#include <stdio.h>
#include <stdbool.h>

/* Max number of source registers for any instruction */
#define SRC_REGISTER_COUNT 2

/* Number of architectural registers */
#define REGISTER_COUNT 128

/* Number of CDBs */
#define CDB_COUNT 1

/* Number of FUs of type 0, 1, 2 */
#define FU0_COUNT 1
#define FU1_COUNT 1
#define FU2_COUNT 1

struct int_register {
	bool ready;
	int tag;
	int val;
};

struct cdb {
	int tag;
	int val;
	int reg_num;
	bool busy;
};

struct reservation_station {
	int fu_type;
	int dest_reg_num;
	int dest_reg_tag;
	struct int_register *src[SRC_REGISTER_COUNT];
};

struct instruction {
	void *addr;
	int fu_type;
	int dest_reg_num;
	int src1_reg_num;
	int src2_reg_num;
};

enum FU_TYPES {
	FU0,
	FU1,
	FU2,
	FU_TYPE_COUNT
};

struct func_unit {
	bool busy;
	int latency;
};

int main(int argc, char const *argv[])
{
	struct reservation_station *sched_queue;
	struct int_register reg_file[REGISTER_COUNT];

	struct cdb csbs[CDB_COUNT];

	struct func_unit fu0[FU0_COUNT];
	struct func_unit fu1[FU1_COUNT];
	struct func_unit fu2[FU2_COUNT];
	struct func_unit *fus[] = {fu0, fu1, fu2};

	printf("Hello world\n");
	return 0;
}
