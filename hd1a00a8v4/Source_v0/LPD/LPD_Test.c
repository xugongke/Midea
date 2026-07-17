#include "SH79F326.h"
#include "intrins.h"
#include "cpu.h"
#include "api_ext.h"
void LPD_Test()
{
  P0CR |= 0x80;
  P0_7 = 0;
  LPDCON = 0x80;	//LPDEN
  LPDSEL = 0xf;		//4.65V
  Delay();
  LPDCON &= 0xef;	//clr LPDIF
  IEN1 |= 0x40;		//ELPD.
  IEN0 |= 0x80;		//EA
  while(1);


}