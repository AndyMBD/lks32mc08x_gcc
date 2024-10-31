/**
 * @file 
 * @copyright (C)2015, LINKO SEMICONDUCTOR Co.ltd
 * @brief 文件名称： lks32mc08x_nvr.h\n
 * 文件标识： 无 \n
 * 内容摘要： NVR驱动头文件 \n
 * 其它说明： 无 \n
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version  <th>Author  <th>Description
 * <tr><td>2018年07月25日 <td>1.0     <td>DengT      <td>创建
 * </table>
 */
#ifndef _LKS32MC08X_NVR_H_
#define _LKS32MC08X_NVR_H_

#include "lks32mc08x.h"

/**
 *@brief 读取NVR 1KB大小的用户区函数
 *@par 示例代码：读取NVR用户地址0的数据
 *@code
        uint32_t LKS_NVRValue = 0;
        LKS_NVRValue = Read_NVR(0x00);//范围0x000~0x400 1KB空间
 *@endcode
 */
uint32_t Read_NVR(uint32_t adr);

/**
 *@brief 读取芯片模块校正值函数
 *@par 示例代码：读取芯片ID
 *@code
        uint32_t LKS_ID1;
        uint32_t LKS_ID2;
        uint32_t LKS_ID3;
        uint32_t LKS_ID4;
        LKS_ID1 = Read_Trim(0x0000084C);
        LKS_ID2 = Read_Trim(0x00000850);
        LKS_ID3 = Read_Trim(0x00000854);
        LKS_ID4 = Read_Trim(0x00000858);
 *@endcode
 */
uint32_t Read_Trim(uint32_t adr);


#endif
/************************ (C) COPYRIGHT LINKO SEMICONDUCTOR **********************/
/* ------------------------------END OF FILE------------------------------------ */
