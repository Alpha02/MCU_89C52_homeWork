#include "..\sameShit.h"
#include "NRF24L01.h"
void main(){
	NRF_Init();
	NRF_Send("compu");
}