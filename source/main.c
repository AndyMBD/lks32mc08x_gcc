#include "basic.h"
#include "lks32mc08x.h"
#include "absacc.h"
#include "hardware_init.h"
static u8 test_data_1=0;
static u8 test_data_2=0;
static u8 test_data_3=0;
int main(void)
{

	Hardware_init();

	for(;;)
	{
		test_data_1=test_data_1+1;
		test_data_2=test_data_2+2;
		test_data_3=test_data_3+3;
		if(test_data_1>250||test_data_2>250||test_data_3>250)
		{
			test_data_1=0;
			test_data_2=0;
			test_data_3=0;
		}
	}
}
