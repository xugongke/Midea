#include "api_ext.h"
#include "SH79F326.h"
void Pca0_Test()
{
	IEN2 |=0x20;//‘ –ÌPCA0÷–∂œ
	IEN0 |= 0x80;
	CLKCON = 0X00;
	select_bank1();
	CEXCR = 0x30;
	init_Pca0cpm0();
	select_bank0();
	while(1);
}