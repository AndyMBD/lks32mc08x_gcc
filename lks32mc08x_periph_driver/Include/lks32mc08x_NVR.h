/**
 * @file 
 * @copyright (C)2015, LINKO SEMICONDUCTOR Co.ltd
 * @brief �ļ����ƣ� lks32mc08x_nvr.h\n
 * �ļ���ʶ�� �� \n
 * ����ժҪ�� NVR����ͷ�ļ� \n
 * ����˵���� �� \n
 *@par �޸���־:
 * <table>
 * <tr><th>Date	        <th>Version  <th>Author  <th>Description
 * <tr><td>2018��07��25�� <td>1.0     <td>DengT      <td>����
 * </table>
 */
#ifndef _LKS32MC08X_NVR_H_
#define _LKS32MC08X_NVR_H_

#include "lks32mc08x.h"

/**
 *@brief ��ȡNVR 1KB��С���û�������
 *@par ʾ�����룺��ȡNVR�û���ַ0������
 *@code
        uint32_t LKS_NVRValue = 0;
        LKS_NVRValue = Read_NVR(0x00);//��Χ0x000~0x400 1KB�ռ�
 *@endcode
 */
uint32_t Read_NVR(uint32_t adr);

/**
 *@brief ��ȡоƬģ��У��ֵ����
 *@par ʾ�����룺��ȡоƬID
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