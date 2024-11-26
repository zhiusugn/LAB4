/*
 * scheduler.c
 *
 *  Created on: Nov 26, 2024
 *      Author: Asus
 */
#include "scheduler.h"

void SCH_Delete_Task(const uint8_t TASK_INDEX){
	SCH_tasks_G[TASK_INDEX].pTask = 0;
	SCH_tasks_G[TASK_INDEX].Delay = 0;
	SCH_tasks_G[TASK_INDEX].Period = 0;
	SCH_tasks_G[TASK_INDEX].Runme = 0;
}

void SCH_Init(){
	uint8_t i;
	for(i = 0; i < SCH_MAX_TASKS; i++){
		SCH_Delete_Task(i);
	}
}

void SCH_Add_Task(void (*pFunction) (), uint32_t DELAY, uint32_t PERIOD){
		DELAY=DELAY/TICK;
		PERIOD=PERIOD/TICK;
		uint8_t index, indexPre;
		//Tim cho trong trong mang de them task vao
		for(index = 0;index < SCH_MAX_TASKS; index++){
			if(SCH_tasks_G[index].pTask == 0) break;
		}
		//Kiem tra neu la phan tu dau tien thi them task vao roi thoat khoi ham
		if(index == 0){
			SCH_tasks_G[index].pTask = pFunction;
			SCH_tasks_G[index].Delay = DELAY;
			SCH_tasks_G[index].Period = PERIOD;
			SCH_tasks_G[index].Runme = 0;
			return;
		}
		//Kiem tra neu mang k con cho trong thi thoat khoi ham
		else if(index == SCH_MAX_TASKS) return;
		else{
			for(int i = 0; i <= index; i++){
				//So sanh delay cua task moi voi cac task trc do trong mang
				if(DELAY >= SCH_tasks_G[i].Delay && SCH_tasks_G[i].pTask) {
					DELAY-=SCH_tasks_G[i].Delay;
				}
				else{
					indexPre = i;
					SCH_tasks_G[i].Delay -= DELAY;
					break;
				}
				indexPre = i;
			}
			//Cap nhat lai vi tri cua cac task
			for(int i = index; i > indexPre; i--){
				SCH_tasks_G[i] = SCH_tasks_G[i-1];
			}
			//Them task
			SCH_tasks_G[indexPre].pTask = pFunction;
			SCH_tasks_G[indexPre].Delay = DELAY;
			SCH_tasks_G[indexPre].Period = PERIOD;
			SCH_tasks_G[indexPre].Runme = 0;
		}
}

void SCH_Update_ListTask(uint8_t index){
	for(uint8_t i = index; i < SCH_MAX_TASKS - 1; i++){
		SCH_tasks_G[i] = SCH_tasks_G[i+1];
	}
		SCH_Delete_Task(SCH_MAX_TASKS-1);
}

void SCH_Dispatch_Tasks(){
	uint8_t index = 0;
	sTask temp;
	for(index = 0; index < SCH_MAX_TASKS; index ++){
		if(SCH_tasks_G[index].Runme){
			(*SCH_tasks_G[index].pTask)();
			SCH_tasks_G[index].Runme -= 1;
			temp = SCH_tasks_G[index];
			//Cap nhat lai list task
			SCH_Update_ListTask(index);
			if(temp.Period){
				SCH_Add_Task(temp.pTask, temp.Period * TICK, temp.Period * TICK);
			}
		}
	}
}

void SCH_Update(){
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        if (SCH_tasks_G[i].pTask) {
            if (SCH_tasks_G[i].Delay == 0) {
                SCH_tasks_G[i].Runme++;
                if (SCH_tasks_G[i].Period > 0) {
                    SCH_tasks_G[i].Delay = SCH_tasks_G[i].Period;
                }
            } else {
                SCH_tasks_G[i].Delay--;
            }
        }
    }
}
