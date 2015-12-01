/**
 * @file     tools.c
 * @brief    一些工具类，包括数组初始化，排序，数组元素交换等
 * @date  2014.4.2
 * @author sinlly
 */
#include "tools.h"
/**
 * @brief 从数组中找到对应的index
 * @param src  原始数组
 * @param  length  数组长度
 * @param value  需要寻找的值
 * @return  value的index
 */
int findIntegerIndex(int src[],int length, int value)
{
   int i;
   for(i=0;i<length;i++)
   {
	   if(value==src[i])
		   return i;
   }
   return -1;
}
/**
 * @brief 初始化float数组
 * @param  src  原始数组
 * @param length 数组长度
 */
void zerofloatArray(float src[],int length)
{
   int i=0;
   for(;i<length;i++)
   {
	   src[i]=0.0;
   }
}
/**
 * @brief 初始化int数组
 * @param  src  原始数组
 * @param length  数组长度
 */
void zeroIntArray(int src[],int length)
{
   int i=0;
   for(;i<length;i++)
   {
	   src[i]=0;
   }
}
/**
 * @brief 将字符转换为整数
 * @param value 需要转换的字符串
 * @return  转换后的整数
 */
int convertStrToInt(char *value)
{
     int res = 0;
     while (*value)
     {
        res = res*10+(*value-'0');
        value++;
     }
     return res;
}

/**
 *@brief  将字符数组通过拆分，拆分部分分别转换为整数,符号两边的数存放在数组中(这里主要用于时间的拆分
 *@param  strTime  需要拆分的字符串
 *@param  time 保存拆分后的时间(小时,分钟,秒)
 *@param  ch  分隔符
 */
void split(char *strTime, int *time, char ch)
{
   char strTemp[10];
   int i = 0;
   int j = 0;
   while(*strTime)
   {
      if ( *strTime == ch )
      {
         strTemp[i] = 0;
         time[j++] = convertStrToInt(strTemp);
         i = 0;
      }
      else
      {
         strTemp[i++] = *strTime;
      }
      strTime++;
   }
   strTemp[i] = 0;
   if ( strlen(strTemp)>0)
   {
       time[j++] = convertStrToInt(strTemp);
   }
}
/**
 * @brief 对float类型数据进行四舍五入到整数
 * @param v  需要四舍五入的数
 * @return  四舍五入后的整数
 */
int roundInt(float v)
{
	return (int)(v+0.5);
}
/**
 * @brief 交换数据
 * @param  src  原始数组
 * @param  i   用于交换的数的下标
 * @param j    用于交换的数的下标
 */
void swap(int src[],int i,int j)
{
	int key=src[i];
	src[i]=src[j];
	src[j]=key;
}
/**
 * @brief 快排序获得分割点
 * @param src  原始数组
 * @param  i   起始index
 * @param  j 终止index
 * @return  分割点的index
 */
int partition(int src[],int i,int j)
{
  int key=src[i];
  int m=i+1;
  int n=j;
  while(m<=n)
  {
	  while(m<=n && src[m]<=key) m++;
	  while(m<=n && src[n]>=key) n--;
	  if(m>=n)
		  break;
      swap(src,m,n);
  }
  src[i]=src[n];
  src[n]=key;
  return n;
}
/**
 * @brief 快速排序
 * @param src  原始数组
 * @param  i  起始index
 * @param  j  终止index
 */
void quickSort(int src[],int i,int j)
{
  if(i<j)
  {
	  int p=partition(src,i,j);
	  quickSort(src,i,p-1);
	  quickSort(src,p+1,j);
  }
}
/**
 * @brief  去除重复点
 * @param src  原始数组
 * @param srcLength 原始数组长度
 * @param dest 目的数组
 * @param destLength  目的数组长度(去除重复点之后的长度)
 */
void unique(int src[],int srcLength,int dest[],int* desLength)
{
	 int i=0;
	 for(;i<srcLength;i++)
	 {
	    if (i==0)
	    	{
	    	    dest[*desLength]=src[i];
	    		*desLength=*desLength+1;
	    	}
	    	else
	    	{
	    		int j=0;
	    		int flag=0;
	    		for(;j<*desLength;j++)
	    		{
	    			if(src[i]==dest[j])
	    			{
	                   flag=1;break;
	    			}
	    		}
	    		if(flag==0)
	    		{
	    			 dest[*desLength]=src[i];
	    			 *desLength=*desLength+1;
	    		}

	    	}
	 }
}
/**
 * @brief 字符串复制操作
 * @param src  原始字符串
 * @param desc   终止字符串
 */
void copy(char* src, char * desc)
{
        int i=0;
        while(desc[i]!='\0')
        {
        	src[i]=desc[i];
        	i++;
        }
        src[i]='\0';
}
