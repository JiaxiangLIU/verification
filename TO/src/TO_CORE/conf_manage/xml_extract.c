#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mxml.h"
#include "xml_extract.h"
#define NUM0 50 //OptimizeStrategy.xml中RawStrategy里Strategy元素的最大个数
#define NUM1 30 //OptimizeStrategy.xml中LimitStrategy里Strategy元素的最大个数;

#define NUM 100 //OptimizeStrategy.xml中TimeStrategy里Strategy元素的最大个数

FILE *cfg_open(int cfg_logo){
	FILE *fd;
	switch(cfg_logo){
	case 0:
		fd = fopen("./RawData/LocoInfo.xml", "r");
	    if(fd == NULL){
	    	perror("LocoInfo.xml not exist");
	    	exit(EXIT_FAILURE);
	    }
		return fd;
		break;
	case 1:
		fd = fopen("./RawData/OptimizeStrategy.xml", "r");
		if(fd == NULL){
			perror("OptimizeStrategy.xml not exist");
			exit(1);
	    }
		return fd;
		break;
	case 2:
		fd = fopen("./RawData/RouteInfo.xml", "r");
		if(fd == NULL){
			perror("RouteInfo.xml not exist");
			exit(1);
	    }
		return fd;
		break;
	case 3:
		fd = fopen("./RawData/roadStationTime.xml", "r");
		if(fd == NULL){
			perror("roadStationTime.xml not exist");
			exit(1);
		}
		return fd;
		break;
	case 4:
		fd = fopen("./RawData/EventConf.xml", "r");
		if(fd == NULL){
			perror("EventConf.xml not exist");
			exit(1);
	    }
		return fd;
		break;
	case 5:
		fd = fopen("./RawData/roadStationTime.xml", "r");
		if(fd == NULL){
			perror("roadStationTime.xml not exist");
			exit(1);
		}
		return fd;
		break;
	}
	return fd;
}


mxml_node_t *cfgload(FILE *fd){
	mxml_node_t *node;
	node = mxmlLoadFile(NULL, fd, MXML_TEXT_CALLBACK);
	    fclose(fd);
		if(node == NULL){
			fprintf(stderr,"mxmloadFile\n");
			exit(EXIT_FAILURE);
		}
	return node;
}


/**
 * 提取LocoInfo.xml文件中的数据
 * */
struct EXTR_ROUTEINFO *extr_LocoInfo(const char *attr_value, int *err_no){
	mxml_node_t *root;
	mxml_node_t *node;
	FILE *fd;
	struct EXTR_ROUTEINFO *rt;
	int num;
	num = 0;
	rt = (struct EXTR_ROUTEINFO *)malloc(sizeof(struct EXTR_ROUTEINFO));
	if (rt == NULL) {
		*err_no = MALLOC_FAILED;
		fprintf(stderr, "MALLOC_FAILED\n");
		exit(EXIT_FAILURE);
	} else {
		rt->RouteInfo_p = (struct PARAMCFG *)malloc(sizeof(struct PARAMCFG) * NUM1);
	}
	if (rt == NULL) {
		*err_no = MALLOC_FAILED;
		fprintf(stderr, "MALLOC_FAILED\n");
		exit(EXIT_FAILURE);
	}
	fd = cfg_open(LOCOINFO);
	if (fd == NULL) {
		*err_no = CFG_OPEN_ERR;
		exit(EXIT_FAILURE);
	}
	root = cfgload(fd);
	if (root == NULL) {
		*err_no = CFG_LOAD_ERR;
		exit(EXIT_FAILURE);
	}
	if (strcmp(attr_value, "locoType") == 0) {
		node = mxmlFindElement(root, root, "Item", "name", attr_value, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR");
			exit(EXIT_FAILURE);
		}
		if (strcmp(node->child->value.text.string, "") != 0) {
			strcpy(rt->RouteInfo_p[num].value, node->child->value.text.string);
		}
	}
	else {
		node = mxmlFindElement(root, root, "Property", "name", attr_value, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR");
			exit(EXIT_FAILURE);
		}
		rt->max = mxmlElementGetAttr(node, "max");
		rt->min = mxmlElementGetAttr(node, "min");
		strcpy(rt->RouteInfo_p[num].value, node->child->value.text.string);
	}

	rt->RouteInfo_len = num;
	num = 0;
	return rt;
}

/**
 *	提取optimizeStrategy.xml中的数据范围
 * */
struct EXTR_OPT_DATARANGE *extr_opt_datarange(const char *element, int *err_no) {
	struct EXTR_OPT_DATARANGE *range;
	range = (struct EXTR_OPT_DATARANGE *)malloc(sizeof(struct EXTR_OPT_DATARANGE));
	if (range == NULL) {
		fprintf(stderr, "malloc err\n");
		*err_no = MALLOC_FAILED;
		exit(EXIT_FAILURE);
	}
	FILE *fd;
	mxml_node_t *root;
	mxml_node_t *node;
	fd = cfg_open(OPTIMIZESTRATEGY);
	if (fd == NULL) {
		*err_no = CFG_OPEN_ERR;
	}
	root = cfgload(fd);
	if (root == NULL) {
		*err_no = CFG_LOAD_ERR;
		fprintf(stderr, "CFG_LOAD_ERR\n");
		exit(EXIT_FAILURE);
	}

	node = mxmlFindElement(root, root, "ParameterConfig", NULL, NULL, MXML_DESCEND);
	if (node == NULL) {
		*err_no = CFG_FIND_ELEMENT_ERR;
		fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
		exit(EXIT_FAILURE);
	}

	node = mxmlFindElement(node, root, "Property", NULL, NULL, MXML_DESCEND);
	if (node == NULL) {
		*err_no = CFG_FIND_ELEMENT_ERR;
		fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
		exit(EXIT_FAILURE);
	}

	for (; node != NULL; node = mxmlFindElement(node, root, "Property", NULL, NULL, MXML_DESCEND)) {
		char* max =mxmlElementGetAttr(node, "max");
		char* min =mxmlElementGetAttr(node, "min");
		if (strcmp(element, (mxmlElementGetAttr(node, "name"))) == 0) {
			range->max=max;
			range->min=min;
			break;
		}
	}
	return range;
}

/**
 *	提取optimizeStrategy.xml中的数据
 * */
struct EXTR_OPT_RTN *extr_OptimizeStrategy (const char *element, int *err_no)
{
	struct EXTR_OPT_RTN *rt;
	rt = (struct EXTR_OPT_RTN *)malloc(sizeof(struct EXTR_OPT_RTN));
	if (rt == NULL) {
		fprintf(stderr, "malloc err\n");
		*err_no = MALLOC_FAILED;
		exit(EXIT_FAILURE);
	}

	rt->strategyFlags_p = (struct strategyFlags *)malloc(sizeof(struct strategyFlags) * NUM0);
	if (rt->strategyFlags_p == NULL){
		fprintf(stderr, "malloc err\n");
		*err_no = MALLOC_FAILED;
		exit(EXIT_FAILURE);
	}

	rt->limitStrategyFLags_p = (struct limitStrategyFLags *)malloc(sizeof(struct limitStrategyFLags) * NUM1);
	if (rt->strategyFlags_p == NULL){
		fprintf(stderr, "malloc err\n");
		*err_no = MALLOC_FAILED;
		exit(EXIT_FAILURE);
	}

	rt->timeStrategyFlags_p = (struct timeStrategyFlags *)malloc(sizeof(struct timeStrategyFlags) * NUM);
	if (rt->strategyFlags_p == NULL){
		fprintf(stderr, "malloc err\n");
		*err_no = MALLOC_FAILED;
		exit(EXIT_FAILURE);
	}



	FILE *fd;
	mxml_node_t *root;
	mxml_node_t *node;
	mxml_node_t *node1;
	int num;
	int len;
	num = 0;
	len = 0;
	fd = cfg_open(OPTIMIZESTRATEGY);
	if (fd == NULL) {
		*err_no = CFG_OPEN_ERR;
	}
	root = cfgload(fd);
	if (root == NULL) {
		*err_no = CFG_LOAD_ERR;
		fprintf(stderr, "CFG_LOAD_ERR\n");
		exit(EXIT_FAILURE);
	}
	if(strcmp(element, "RawStrategy") == 0){
		node = mxmlFindElement(root, root, "RawStrategy", NULL, NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
			exit(EXIT_FAILURE);
		}

		len = atoi(mxmlElementGetAttr(node,"len"));
//		printf("RawStrategy len = %d\n", len);
		node = mxmlFindElement(node, root, "Strategy", NULL, NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
			exit(EXIT_FAILURE);
		}

		for (; node != NULL;
				node = mxmlFindElement(node, root, "Strategy", NULL, NULL, MXML_DESCEND)){

			rt->strategyFlags_p[num].gradientType = atoi(mxmlElementGetAttr(node,"flag"));
			strcpy(rt->strategyFlags_p[num].name, mxmlElementGetAttr(node,"name"));

			node1 = mxmlFindElement(node, root, "Weight", NULL, NULL, MXML_DESCEND);
			rt->strategyFlags_p[num].minMass = atoi(mxmlElementGetAttr(node1,"min"));
			rt->strategyFlags_p[num].maxMass = atoi(mxmlElementGetAttr(node1,"max"));

			node1 = mxmlFindElement(node, root, "Length", NULL, NULL, MXML_DESCEND);
			rt->strategyFlags_p[num].minLen = atoi(mxmlElementGetAttr(node1,"min"));
			rt->strategyFlags_p[num].maxLen = atoi(mxmlElementGetAttr(node1,"max"));

			node1 = mxmlFindElement(node, root, "AvgV", NULL, NULL, MXML_DESCEND);
			rt->strategyFlags_p[num].minAvg = atoi(mxmlElementGetAttr(node1,"min"));
			rt->strategyFlags_p[num].maxAvg = atoi(mxmlElementGetAttr(node1,"max"));

			node1 = mxmlFindElement(node, root, "PartGradFlag", NULL, NULL, MXML_DESCEND);
			rt->strategyFlags_p[num].partGradFlag = atoi(mxmlElementGetAttr(node1,"flag"));

			node1 = mxmlFindElement(node, root, "SteepUpPect", NULL, NULL, MXML_DESCEND);
			rt->strategyFlags_p[num].minSteepUpPect = atoi(mxmlElementGetAttr(node1,"min"));
			rt->strategyFlags_p[num].maxSteepUpPect = atoi(mxmlElementGetAttr(node1,"max"));

			node1 = mxmlFindElement(node, root, "SteepDownPect", NULL, NULL, MXML_DESCEND);
			rt->strategyFlags_p[num].minSteepDownPect = atoi(mxmlElementGetAttr(node1,"min"));
			rt->strategyFlags_p[num].maxSteepDownPect = atoi(mxmlElementGetAttr(node1,"max"));

			node1 = mxmlFindElement(node, root, "LastGraType", NULL, NULL, MXML_DESCEND_FIRST);
			if (node1 != NULL) {
				rt->strategyFlags_p[num].lastGraType = atoi(mxmlElementGetAttr(node1,"flag"));

			}
			else {
				rt->strategyFlags_p[num].lastGraType = 100;
			}

			node1 = mxmlFindElement(node, root, "NextGraType", NULL, NULL, MXML_DESCEND_FIRST);
			if (node1 != NULL) {
				rt->strategyFlags_p[num].nextGraType = atoi(mxmlElementGetAttr(node1,"flag"));
			}
			else {
				rt->strategyFlags_p[num].nextGraType = 100;
			}

			num += 1;
			if (num == len){//如果不加这条if判断，for循环到最后会出错，node并没有指向NULL
				break;
			}
		}
		rt->strategyFlags_len = num;
		num = 0;
#if 1
//		printf("strategyFlags_len = %d\n", rt->strategyFlags_len);
#endif
	}

	else if (strcmp(element, "LimitStrategy") == 0) {
		node = mxmlFindElement(root, root, "LimitStrategy", NULL, NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
			exit(EXIT_FAILURE);
		}

		len = atoi(mxmlElementGetAttr(node,"len"));
#if 1
//		printf("LimitStrategy len = %d\n", len);
#endif
		node = mxmlFindElement(node, root, "Strategy", NULL, NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
			exit(EXIT_FAILURE);
		}

		for (; node != NULL; \
				node = mxmlFindElement(node, root, "Strategy", NULL, NULL, MXML_DESCEND)) {
			rt->limitStrategyFLags_p[num].type = atoi(mxmlElementGetAttr(node,"id"));
			strcpy(rt->limitStrategyFLags_p[num].name, mxmlElementGetAttr(node,"name"));
			node1 = mxmlFindElement(node, root, "Weight", NULL, NULL, MXML_DESCEND);
			rt->limitStrategyFLags_p[num].maxMass = atoi(mxmlElementGetAttr(node1, "max"));
			rt->limitStrategyFLags_p[num].minMass = atoi(mxmlElementGetAttr(node1, "min"));
			num += 1;
#if 1
//			printf("num = %d\n", (num - 1));
#endif
			if (num == len){
				break;
			}
		}
		rt->limitStrategyFLags_len = num;
		num = 0;
//		printf("limitStrategyFLags_len = %d\n", rt->limitStrategyFLags_len);
	}

	else if (strcmp(element, "TimeStrategy") == 0) {
		node = mxmlFindElement(root, root, "TimeStrategy", NULL, NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
			exit(EXIT_FAILURE);
		}

		len = atoi(mxmlElementGetAttr(node,"len"));
#if 1
//		printf("TimeStrategy len = %d\n", len);
#endif
		node = mxmlFindElement(node, root, "Strategy", NULL, NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
			exit(EXIT_FAILURE);
		}

		for (; node != NULL; \
				node = mxmlFindElement(node, root, "Strategy", NULL, NULL, MXML_DESCEND)) {
			rt->timeStrategyFlags_p[num].direction = atoi(mxmlElementGetAttr(node,"direction"));
			rt->timeStrategyFlags_p[num].leftRType = atoi(mxmlElementGetAttr(node,"leftType"));
			rt->timeStrategyFlags_p[num].rightRType = atoi(mxmlElementGetAttr(node,"rightType"));
			rt->timeStrategyFlags_p[num].singleR = atoi(mxmlElementGetAttr(node,"singleType"));
			rt->timeStrategyFlags_p[num].up = atoi(mxmlElementGetAttr(node,"vUp"));
			strcpy(rt->timeStrategyFlags_p[num].name, mxmlElementGetAttr(node,"name"));
			num += 1;
#if 0
			printf("num = %d\n", (num - 1));
#endif
			if (num == len){
				break;
			}
		}
		rt->timeStrategyFlags_len = num;
		num = 0;
#if 1
//		printf("timeStrategyFlags_len = %d\n", rt->timeStrategyFlags_len);
#endif
	}

	else {
		node = mxmlFindElement(root, root, "ParameterConfig", NULL, NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
			exit(EXIT_FAILURE);
		}

		node = mxmlFindElement(node, root, "Property", NULL, NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
			fprintf(stderr, "CFG_FIND_ELEMENT_ERR\n");
			exit(EXIT_FAILURE);
		}

		for (; node != NULL; node = mxmlFindElement(node, root, "Property", NULL, NULL, MXML_DESCEND)) {
			if (strcmp(element, (mxmlElementGetAttr(node, "name"))) == 0) {
				strcpy(rt->PrmtCfg_value, node->child->value.text.string);
				break;
			}
		}
	}
	return rt;
}


struct ROUTEINFO *extr_RouteInfo (const char *element, int *err_no)
{
	struct ROUTEINFO *rt;
	rt = (struct ROUTEINFO *)malloc(sizeof(struct ROUTEINFO));
	if (rt == NULL) {
		printf("malloc err.\n");
		*err_no = MALLOC_FAILED;
		exit(EXIT_FAILURE);
	}

	rt->in_routeinfo = (struct IN_ROUTEINFO *)malloc(sizeof(struct IN_ROUTEINFO) * NUM1);
	if (rt == NULL) {
		printf("malloc err.\n");
		*err_no = MALLOC_FAILED;
		exit(EXIT_FAILURE);
	}

	FILE *fd;
	mxml_node_t *root;
	mxml_node_t *node;
	int num;
//	int len;
	num = 0;
//	len = 0;
	fd = cfg_open(ROUTEINFO);
	root = cfgload(fd);
	node = mxmlFindElement(root, root, "Item", "name", element, MXML_DESCEND);
	if (node == NULL) {
		*err_no = CFG_FIND_ELEMENT_ERR;
	}

	if (strcmp("pusherEngine", element) == 0) {
		node = mxmlFindElement(root, root, "Part", "num", NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		}

		for (; node != NULL; node = mxmlFindElement(node, root, "Part", "num", NULL, MXML_DESCEND)) {

			node = mxmlFindElement(node, root, "Property", "name", "direction", MXML_DESCEND);
			if (node == NULL) {
				*err_no = CFG_FIND_ELEMENT_ERR;
			} else {
				rt->in_routeinfo[num].direction = atoi( node->child->value.text.string);
			}
			node = mxmlFindElement(node, root, "Property", "name", "endTelesemes", MXML_DESCEND);
			if (node == NULL) {
				*err_no = CFG_FIND_ELEMENT_ERR;
			} else {
				rt->in_routeinfo[num].endTelesemes = atoi( node->child->value.text.string);
			}
			node = mxmlFindElement(node, root, "Property", "name", "endPosOffset", MXML_DESCEND);
			if (node == NULL) {
				*err_no = CFG_FIND_ELEMENT_ERR;
			} else {
				rt->in_routeinfo[num].endPosOffset = atoi( node->child->value.text.string);
			}
			node = mxmlFindElement(node, root, "Property", "name", "id", MXML_DESCEND);
			if (node == NULL) {
				*err_no = CFG_FIND_ELEMENT_ERR;
			} else {
				rt->in_routeinfo[num].id = atoi( node->child->value.text.string);
			}
			node = mxmlFindElement(node, root, "Property", "name", "linenum", MXML_DESCEND);
			if (node == NULL) {
				*err_no = CFG_FIND_ELEMENT_ERR;
			} else {
				rt->in_routeinfo[num].linenum = atoi( node->child->value.text.string);
			}
			node = mxmlFindElement(node, root, "Property", "name", "startTelesemes", MXML_DESCEND);
			if (node == NULL) {
				*err_no = CFG_FIND_ELEMENT_ERR;
			} else {
				rt->in_routeinfo[num].startTelesemes = atoi( node->child->value.text.string);
			}
			node = mxmlFindElement(node, root, "Property", "name", "startPosOffset", MXML_DESCEND);
			if (node == NULL) {
				*err_no = CFG_FIND_ELEMENT_ERR;
			} else {
				rt->in_routeinfo[num].startPosOffset = atoi( node->child->value.text.string);
			}
			node = mxmlFindElement(node, root, "Property", "name", "type", MXML_DESCEND);
			if (node == NULL) {
				*err_no = CFG_FIND_ELEMENT_ERR;
			} else {
				rt->in_routeinfo[num].type = atoi( node->child->value.text.string);
			}
			num += 1;
		}
		rt->part_num = num;
		num = 0;
	}
	else if (strcmp("throughExpert", element) == 0) {
		node = mxmlFindElement(node, root, "Property", "name", "direction", MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		} else {
			rt->in_routeinfo[num].direction = atoi( node->child->value.text.string);
		}
		node = mxmlFindElement(node, root, "Property", "name", "endTelesemes", MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		} else {
			rt->in_routeinfo[num].endTelesemes = atoi( node->child->value.text.string);
		}
		node = mxmlFindElement(node, root, "Property", "name", "endPosOffset", MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		} else {
			rt->in_routeinfo[num].endPosOffset = atoi( node->child->value.text.string);
		}
		node = mxmlFindElement(node, root, "Property", "name", "id", MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		} else {
			rt->in_routeinfo[num].id = atoi( node->child->value.text.string);
		}
		node = mxmlFindElement(node, root, "Property", "name", "linenum", MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		} else {
			rt->in_routeinfo[num].linenum = atoi( node->child->value.text.string);
		}
		node = mxmlFindElement(node, root, "Property", "name", "startTelesemes", MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		} else {
			rt->in_routeinfo[num].startTelesemes = atoi( node->child->value.text.string);
		}
		node = mxmlFindElement(node, root, "Property", "name", "startPosOffset", MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		} else {
			rt->in_routeinfo[num].startPosOffset = atoi( node->child->value.text.string);
		}
		node = mxmlFindElement(node, root, "Property", "name", "type", MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		} else {
			rt->in_routeinfo[num].type = atoi( node->child->value.text.string);
		}

		node = mxmlFindElement(node, root, "Property", "name", "endV", MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		}
		rt->in_routeinfo[num].endV = atof( node->child->value.text.string);
	}
	else {
		node = mxmlFindElement(root, root, "Item", "name", NULL, MXML_DESCEND);
		if (node == NULL) {
			*err_no = CFG_FIND_ELEMENT_ERR;
		}
		for (; node != NULL; node = mxmlFindElement(node, root, "Property", "name", NULL, MXML_DESCEND)) {
			if (strcmp(element, (mxmlElementGetAttr(node, "name"))) == 0) {
				strcpy(rt->value, node->child->value.text.string);
				rt->max = mxmlElementGetAttr(node, "max");
				rt->min = mxmlElementGetAttr(node, "min");
				break;
			}
		}
	}
	*err_no = CFG_EXTR_SUCCESS;
	return rt;
}

struct rt_extrRoadStationTime *extrRoadStationTime(int *err_no)
{
	struct rt_extrRoadStationTime *timetable_t;
	int num;
	FILE *fd;
	mxml_node_t *root;
	mxml_node_t *item;
	mxml_node_t *item_child;
	int statNo;
	int tmisNo;
	int routeNo;

	num = 0;
	timetable_t = (struct rt_extrRoadStationTime *)malloc(sizeof(struct rt_extrRoadStationTime));
	if(timetable_t == NULL){
		fprintf(stderr, "malloc failed\n");
		*err_no = MALLOC_FAILED;
		exit(EXIT_FAILURE);
	}

	timetable_t->ptr = (struct EXTRACT_FR_STT *)malloc(sizeof(struct EXTRACT_FR_STT) * NUM1);
	if(timetable_t->ptr == NULL){
		fprintf(stderr, "malloc failed\n");
		*err_no = MALLOC_FAILED;
		exit(EXIT_FAILURE);
	}

	fd = cfg_open(ROADSTATIONTIME);
	if (fd == NULL) {
		*err_no = CFG_OPEN_ERR;
	}
	root = cfgload(fd);
	if (root == NULL) {
		*err_no = CFG_LOAD_ERR;
	}
	if(root == NULL){
		fprintf(stderr, "cfgload() err!\n");
		exit(EXIT_FAILURE);
	}
	item = mxmlFindElement(root, root, "Item", NULL, NULL, MXML_DESCEND);
	if(item == NULL){
		fprintf(stderr, "mxmlFindElement() err!\n");
		exit(EXIT_FAILURE);
	}
	for (; item != NULL; item = mxmlFindElement(item, root, "Item", NULL, NULL,MXML_DESCEND)){
/*检索"station","tmis_num","route_num"*/
		statNo = atoi(mxmlElementGetAttr(item,"station"));
        tmisNo = atoi(mxmlElementGetAttr(item,"tmis_num"));
        routeNo = atoi(mxmlElementGetAttr(item,"route_num"));
        timetable_t->ptr[num].station = statNo;
        timetable_t->ptr[num].tmis = tmisNo;
        timetable_t->ptr[num].route_num = routeNo;

/*检索“ArriveTime”,"StartTime","NextStation"*/
        item_child= mxmlFindElement(item, root, "ArriveTime", NULL, NULL, MXML_DESCEND_FIRST);
        if (item_child != NULL) {
       		strcpy(timetable_t->ptr[num].arrivetime, item_child->child->value.text.string);
        }
        else {
        	strcpy(timetable_t->ptr[num].arrivetime, "nothing");
        }

        item_child= mxmlFindElement(item, root, "StartTime", NULL, NULL, MXML_DESCEND_FIRST);
        if (item_child != NULL) {
       		strcpy(timetable_t->ptr[num].starttime, item_child->child->value.text.string);
        }
        else {
           	strcpy(timetable_t->ptr[num].starttime, "nothing");
        }

        item_child= mxmlFindElement(item, root, "NextStation", NULL, NULL, MXML_DESCEND_FIRST);
        if (item_child != NULL) {
       		strcpy(timetable_t->ptr[num].netxstation, item_child->child->value.text.string);
        }
        else {
           	strcpy(timetable_t->ptr[num].netxstation, "nothing");
        }

        num = num + 1;

//        if (item == NULL){
//			break;
//		}
	}
	timetable_t->ptr_size = num;
	num = 0;
	return timetable_t;
}


int delet(void *return_type)
{
	free(return_type);
	return 0;
}
