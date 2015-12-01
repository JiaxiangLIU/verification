/**
 * @file     tools.h
 * @brief    一些工具类接口，包括数组初始化，排序，数组元素交换等
 * @date  2014.4.2
 * @author sinlly
 */

#ifndef TOOLS_H_
#define TOOLS_H_

/**
 * @brief 找到数组中和value相等的值的index,若没有，则返回-1
 * @param  src 源数组
 * @param  length 源数组长度
 * @param  value 需要寻找的值
 * @return 返回数组中存在value值对应的index
 */
int findIntegerIndex(int src[],int length,int value);
/**
 * @brief 对数组进行初始化数据为0
 * @param  src 源数组
 * @param  length 源数组长度
 */
void zerofloatArray(float src[],int length);
/**
 * @brief 将字符数组转换为整数
 * @param  value 需要转换的字符数组  如'23'转换为23
 * @return  字符对应的整数
 */
int convertStrToInt(char *value);
/**
 * @brief 将字符数组通过分隔符分隔开
 * @param  strTime 需要分割的字符数组  如'23:12'
 * @param  time  分割后的部分保存在该数组中 '23','12'
 * @param  ch 分隔符  ':'
 */
void split(char *strTime, int *time, char ch);
/**
 * @brief  将float类型数字四舍五入
 * @param  v  需要四舍五入的数字
 * @return  四舍五入之后的数字
 */
int roundInt(float v);
/**
 * @brief 将整型数组初始化为0
 * @param  src 整型数组
 * @param  length 源数组长度
 */
void zeroIntArray(int src[],int length);
/**
 * @brief 快速排序
 * @param  src   待排序的整型数组
 * @param  i  数组起始index 一般为0
 * @param  j  数组的终止index  一般为length-1
 */
void quickSort(int src[],int i,int j);
/**
 * @brief 快排序获得分割点
 * @param src  原始数组
 * @param  i   起始index
 * @param  j 终止index
 * @return  分割点的index
 */
int partition(int src[],int i,int j);
/**
 * @brief 数据交换
 * @param src 需要交换的数组
 * @param i  交换的数的index
 * @param j  交换的数的index
 */
void swap(int src[],int i,int j);//交换
/**
 *@brief  去除数组中的重复顶点
 *@param  src 源数组
 *@param  srcLength 源数组长度
 *@param  dest 去除重复点后的数组
 *@param  destLength 去除重复点后的数组长度
 */
void unique(int src[],int srcLength,int dest[],int* desLength);//去除重复的点
/**
 * @brief 将目标字符串复制到源字符串
 * @param src 源字符串
 * @param desc 目标字符串
 */
void copy(char* src, char * desc);

#endif /* TOOLS_H_ */
