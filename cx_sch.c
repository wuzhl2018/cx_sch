/**
  ******************************************************************************
  * @file    cx_sch.c
  * @author  CX
  * @version V1.0.0.1
  * @date    2016-7-26
  * @brief   1.0.0.1
	     修改任务数据结构,增加一级抢占任务
	     加入RunNum预编译选项，限制是否开启任务执行次数
	     1.0.0.0 
	     完成基础架构搭建
  ******************************************************************************
  * @attention
  *
  * 项目   ：None
  * 官网   : None
  * 实验室 ：None
  ******************************************************************************
  */


#include "cx_sch.h"



sTask_Typedef SCH_tasks_G[SCH_MAX_TASKS];



/**
  * @brief   增加任务
  * @param   pFunction 任务指针, Delay 延迟时标, Peroid 任务执行周期, RunNum 任务执行次数, ModeEnum 任务模式
  * @retval  任务队列索引
  * @notice  None
*/
uint8_t SCH_Add_Task(void(*pFunction)(), uint32_t Delay, uint32_t Peroid, uint8_t RunNum, TaskMode_Enum ModeEnum)
{
	uint8_t Index = 0;
	while((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS))	
	{
		Index++;                                                             
	}
	if(Index == SCH_MAX_TASKS)
	{
		return SCH_MAX_TASKS;
	}
	SCH_tasks_G[Index].pTask = pFunction;
	SCH_tasks_G[Index].Delay = Delay;
	SCH_tasks_G[Index].Peroid = Peroid;
	SCH_tasks_G[Index].RunMe = 0;
#if RunNum_ON
	SCH_tasks_G[Index].RunNum = RunNum;
#endif
	SCH_tasks_G[Index].ModeEnum = ModeEnum;
	return Index;
}



/**
  * @brief   删除任务
  * @param   Index, 任务队列索引
  * @retval  None
  * @notice  None
*/
void SCH_Delete_Tasks(uint8_t Index)
{
	SCH_tasks_G[Index].pTask = 0;
	SCH_tasks_G[Index].Delay = 0;
	SCH_tasks_G[Index].Peroid = 0;
	SCH_tasks_G[Index].RunMe = 0;
#if RunNum_ON
	SCH_tasks_G[Index].RunNum = 0;
#endif
}



/**
  * @brief   更新任务
  * @param   None
  * @retval  None
  * @notice  需要心跳支持, 抢占任务执行时长必须小于任务调度器的时标
*/
void SCH_Update_Tasks(void)
{
	u8 Index;
	for(Index = 0;Index < SCH_MAX_TASKS;Index++)
	{
		if(SCH_tasks_G[Index].pTask)
		{
			if(SCH_tasks_G[Index].Delay == 0)   
			{
				switch(SCH_tasks_G[Index].ModeEnum)
				{
					case SEIZ_Enum:                         //抢占任务立即运行
						SCH_tasks_G[Index].pTask();
						if(SCH_tasks_G[Index].RunMe > 0)
						{
							SCH_tasks_G[Index].RunMe--;
#if RunNum_ON
							if(SCH_tasks_G[Index].RunNum > 0)
							{
								SCH_tasks_G[Index].RunNum--;
								if(SCH_tasks_G[Index].RunNum == 0)
								{
									SCH_Delete_Tasks(Index);
								}	
							}
#endif
						}
						break;
					case COOP_Enum:
						SCH_tasks_G[Index].RunMe++;     
						break;
					default:break;
				}				
				if(SCH_tasks_G[Index].Peroid)
				{
					SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Peroid;
				}
			}
			else
			{
				SCH_tasks_G[Index].Delay--;     
			}
		}
	}
}



/**
  * @brief   任务调度器
  * @param   None
  * @retval  None
  * @notice  None
*/
void SCH_Dispatch_Tasks(void)
{
	u8 Index;
	while(1)
	{
		for(Index = 0;Index < SCH_MAX_TASKS;Index++)
		{
			if(SCH_tasks_G[Index].RunMe > 0)
			{
				SCH_tasks_G[Index].pTask();
				SCH_tasks_G[Index].RunMe--;
#if RunNum_ON
				if(SCH_tasks_G[Index].RunNum > 0)
				{
					SCH_tasks_G[Index].RunNum--;
					if(SCH_tasks_G[Index].RunNum == 0)
					{
						SCH_Delete_Tasks(Index);
					}	
				}
#endif
				if(SCH_tasks_G[Index].Peroid == 0)
				{
					SCH_Delete_Tasks(Index);
				}
			}
		}
	}
}



/**
  * @brief   任务初始化
  * @param   None
  * @retval  None
  * @notice  None
*/
void SCH_Init(void)
{
	TIM2Base_Config(10);
}


/**
  * @brief   启动任务调度
  * @param   None
  * @retval  None
  * @notice  None
*/
void SCH_Start(void)
{
	TIM_Cmd(TIM2, ENABLE);	
}
