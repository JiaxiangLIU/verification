/*
 * main.c
 * 程序入口函数
 *  Created on: 2014年4月1日
 *      Author: sinllychen
 */
#include "opt_global_var.h"
#include "optimize_curve.h"
#include "extrData.h"
#include "exception.h"


int MAX_OPTSCHLENGTH;

int opt_init()
{
	OPTSCHCURVE* final_curve;

    final_curve=optimize(1);

   // tempLimitOpimize(104604.578125,107375.828125,50,new_final_curve,MAX_FINAL_LENGTH);


 

//    int i;
//    FILE* fid2 = fopen("././Test/Test_finalResult.txt", "w");
//    for(i=0;i<MAX_FINAL_LENGTH;i++){
//    			fprintf(fid2, "%d：\t%f\t%f\t%d\t%d\t  \n",i,new_final_curve[i].start,new_final_curve[i+1].start,new_final_curve[i].gear,new_final_curve[i+1].gear);
//
//    }
//    fclose(fid2);



//	 tempLimitOpimize(83077-11457.000000,83077- 12218.472656,50,optSch);
//    printf("%f,%f",optSch[106499].start,optSch[106499].velocity);

//	running_train();
}

