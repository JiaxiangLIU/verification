#include <stdio.h>
#include <stdlib.h>
#include <mxml.h>
#include "dataStruct.h"
#include "cfgFileRead.h"
#define NUM 30//定义列车时刻表信息条目数的最大值，若实际列车时刻表信息的条目数大于30，NUM须重新定义


/*
 * 功能:提取列车时刻表相关信息
 * 参数:findErr,函数extrRoadStationTime()内部调用出错时，修改*findErr的值，extrRoadStationTime()\
 * 		的调用者通过判断*findErr的值，可以知道extrRoadStationTime()内部发生了什么错误
 * 返回值:结构体指针，该结构体的成员为存放列车时刻表信息的结构体的指针和列车时刻表信息的条目数
 */
struct rt_extrRoadStationTime *extrRoadStationTime(int *findErr)
{
	struct rt_extrRoadStationTime *rt;
	int num;
	FILE *fp;
	mxml_node_t *root;
	mxml_node_t *item;
	int statNo;
	int tmisNo;
	int routeNo;
	int bifurcation_distance;
	int bifurcation_speed;

	num = 0;
	rt = (struct rt_extrRoadStationTime *)malloc(sizeof(struct rt_extrRoadStationTime));
	if(rt == NULL){
		fprintf(stderr, "malloc0 failed\n");
    	*findErr = MALLOC_FAILED;
		return NULL;
	}
	rt->ptr = (struct EXTRACT_FR_STT *)malloc(sizeof(struct EXTRACT_FR_STT) * NUM);
	if(rt->ptr == NULL){
		fprintf(stderr, "malloc1 failed\n");
		*findErr = MALLOC_FAILED;
		return NULL;
	}
    fp = fopen("./RawData/roadStationTime.xml", "r");
    if(fp == NULL){
    	perror("fopen");
    	*findErr = CFG_OPEN_ERR;
    	return NULL;

    }
    root = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    fclose(fp);
	if(root == NULL){
		fprintf(stderr, "mxmlLoadFile() err!\n");
		*findErr = CFG_LOAD_ERR;
		return NULL;
	}
	item = mxmlFindElement(root, root, "Item", NULL, NULL, MXML_DESCEND);
	if(item == NULL){
		fprintf(stderr, "mxmlFindElement() err!\n");
		*findErr = CFG_FIND_ELEMENT_ERR;
		return NULL;
	}
	for(; item != NULL; item = mxmlFindElement(item, root, "Item", NULL, NULL,MXML_DESCEND)){
		statNo = atoi(mxmlElementGetAttr(item,"station"));
        tmisNo = atoi(mxmlElementGetAttr(item,"tmis_num"));
        routeNo = atoi(mxmlElementGetAttr(item,"route_num"));
		bifurcation_distance = atoi(mxmlElementGetAttr(item,"bifurcation_distance"));
		bifurcation_speed = atoi(mxmlElementGetAttr(item,"bifurcation_speed"));
        rt->ptr[num].station = statNo;
        rt->ptr[num].tmis = tmisNo;
        rt->ptr[num].route_num = routeNo;
		rt->ptr[num].bifurcation_distance = bifurcation_distance;
		rt->ptr[num].bifurcation_speed = bifurcation_speed;
        num = num + 1;
        if(item == NULL){
			break;
		}
	}
	rt->ptr_size = num;
#if 0
	printf("ptr_size = %d\n", rt->ptr_size);
#endif
	return rt;
}

/*
 * 用于释放调用extrRoadStationTime()函数后得到的内存空间
 */
void delete(struct rt_extrRoadStationTime *pt)
{
	free(pt);
	pt = NULL;//防止pt变成野指针
}
