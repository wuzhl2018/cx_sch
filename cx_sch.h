#ifndef __CX_SCH_H
#define __CX_SCH_H


#include "stm32f10x.h"
#include "cx_timbase.h"


#define      RunNum_ON          0


typedef enum
{
	COOP_Enum = 0x0,
	SEIZ_Enum = 0x1
}TaskMode_Enum;




typedef struct
{
	void (*pTask)(void); 
	uint32_t Delay;			       
	uint32_t Peroid;		     
	uint8_t RunMe;		               //标记任务就绪
#if RunNum_ON
	uint8_t RunNum;                        //指定任务执行次数，执行完自动销毁
#endif
	TaskMode_Enum ModeEnum;
}sTask_Typedef;




#define      SCH_MAX_TASKS         10


void SCH_Init(void);
void SCH_Start(void);
uint8_t SCH_Add_Task(void(*pFunction)(), uint32_t Delay, uint32_t Peroid, uint8_t RunNum, TaskMode_Enum ModeEnum);
void SCH_Update_Tasks(void);
void SCH_Dispatch_Tasks(void);
void SCH_Delete_Tasks(uint8_t Index);



#endif
