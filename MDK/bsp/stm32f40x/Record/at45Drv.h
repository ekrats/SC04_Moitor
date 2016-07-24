/*****************************************************************************
*
* Copyright (c) 2007,深圳通业科技有限公司 .All rights reserved.
*
* -----------------------------------------------------------------------------
*
* 文件摘要:鸣笛监控装置的flash 或IC 卡驱动头文件，ATMEL公司的
* AT45DXX等大容量FLASH /IC卡 的读写程序
*
* -----------------------------------------------------------------------------
*
* 文件名称:	At45drv.h
* 编译环境:	ads1.2
* 最新版本:  v1.0
* 硬件平台:  鸣笛监控V1.0 LPC213X
*****************************************************************************/

/***修改及版本记录*****************************************************************
* 修改时间:  2007/7/28
* 修改人员: 黄楚雄
* 升级版本:	V1.0
* 内容描述: 初次编写
*
**********************************************************************************/
/***修改及版本记录*****************************************************************
* 修改时间:  
* 修改人员:
* 升级版本:
* 内容描述:
*
**********************************************************************************/






//*****************************************************************************
// <<< Use Configuration Wizard in Context Menu >>>
//       ---- 以上注释语句必须添加，才可以启动MDK的可视化宏配置 ----
//*****************************************************************************






#ifndef _AT45DRV_H_
#define _AT45DRV_H_

//#include "stm32f10x_lib.h"
#include "Bsp_Board_Init.h"

/*---------------------------------------------------define------------------------------------------------------------------------*/

#ifdef _AT45DRV_
#define AT45DRV_GM 
#else
#define AT45DRV_GM extern
#endif

#define OpCmdADD 1
#define OpCmdNEW 2
#define OpCmdADDCLR 3	//与上面指令的区别是清楚零后面数据
#define OpCmdNEWCLR 4

#define  SECTOR_0A    0 //扇区0A
#define  SECTOR_0B    0 //扇区0B
/*---------------------------------------------------typedef------------------------------------------------------------------------*/
//AT45DFLASH的操作命令定义
typedef enum 
{  
	PageRead = 0xd2,                 //直接读页，缓存不变
	Buf1Read = 0xd4,                 //缓存1读
	Buf2Read = 0xd6,                 //缓存2读
	PageToBuf1 = 0x53,               //页读至缓存1
	PageToBuf2 = 0x55,               //页读至缓存2
	PageCmpBuf1 = 0x60,              //页与缓存1比较，比较结果在状态寄存器第6位
	PageCmpBuf2 = 0x61,              //页与缓存2比较，比较结果在状态寄存器第6位
	SectorErase = 0x7C,	 		     //扇区擦除
	Buf1Write = 0x84,                //缓存1写
	Buf2Write = 0x87,                //缓存2写
	Buf1ToPageWithErase = 0x83,      //将缓存1写入页,包含页擦除动作
	Buf2ToPageWithErase = 0x86,      //将缓存2写入页,包含页擦除动作
	Buf1ToPageWithoutErase = 0x88,   //将缓存1写入页,不包含页擦除动作
	Buf2ToPageWithoutErase = 0x89,   //将缓存2写入页,不包含页擦除动作
	PageErase = 0x81,                //擦除页面
	PageWriteThroughBuf1 = 0x82,     //数据先写入缓存1，再写入页,包含页擦除
	PageWriteThroughBuf2 = 0x85,     //数据先写入缓存2，再写入页,包含页擦除
	PageRewriteThroughBuf1 = 0x58,   //页读入缓存1，然后缓存1写入页
	PageRewriteThroughBuf2 = 0x59,   //页读入缓存2，然后缓存2写入页
	StatusRead = 0xd7,               //读状态寄存器
	IDRead = 0x9f,	       	         //读芯片ID号

	DeepPowerDown = 0xB9,			 //进入省电模式
	ResumeDeepPowerDown= 0xAB,		 //退出省电模式
	BlockErase = 0x50
} OpCode;

typedef struct _ID
{
	u8 mid;
	u8 did1;
	u8 did2;
	u8 einf;
	u8 sr;
}_ICID;

typedef enum 
{  
	Erase_0_Sector = 0,          //擦除0扇区
	Erase_other_Sector = 1,      //擦除1-15扇区
}EraseSectorProperty;		     //扇区擦除属性定义
//------------------------------------------------------------------
// life add code 2010-5-25
//------------------------------------------------------------------
typedef enum
{
	AT45_FLASH_COMPLETE   = 1,
	AT45_FLASH_OP_TIMEOUT = 0,
}AT45FlashStatus;
/*---------------------------------------------------variable------------------------------------------------------------------------*/

AT45DRV_GM _ICID ICID;

/*---------------------------------------------------function------------------------------------------------------------------------*/
u8 ICInit(void);                  /*初始化IC卡接口*/
/****************************************************************************
** 函数名称：u8 ICWriteToPage(u8 *pData,u16 Page, u16 ByteAddr, u16 Size, u8 Operation)
** 功    能：页写操作函数
** 说    明： 
** 入口参数：p内容 count数量 page 写的页面byteaddr 写的字节地址
operation 操作方式，有ADD和NEW，追加数据或者全新数据
** 出口参数：写成功与否
** 全局变量：
** 作　者：黄楚雄
** 日　期：2007年8月16日
**---------------------------------------------------------------------------
** 修改人：
** 日　期：
** 内　容：
****************************************************************************/
u8 ICWriteToPage(u8 *pData, u16 Page, u16 ByteAddr,u16 Size, u8 Operation);/* 将字符串写入页*/
void ICWriteFullPage(u8 *pData, u16 page, u8 bufferIndex);
/****************************************************************************
** 函数名称：void ICReadPage(u8 *pData, u16 Page, u16 ByteAddr, u16 size)
** 功    能：FLASH直接页读取
** 说    明：根据页面和地址，读取一定数量的数据返回 
** 入口参数：*pData读取返回的数据,  page 读的页面byteaddr 读的字节起始地址
size读取的字节数量
** 出口参数：无
** 全局变量：
** 作　者：黄楚雄
** 日　期：2007年8月16日
**---------------------------------------------------------------------------
** 修改人：
** 日　期：
** 内　容：
****************************************************************************/
void ICReadPage(u8 *pData, u16 Page, u16 ByteAddr, u16 Size);/*直接从FLASH读取数据*/
/*直接读页内容*/
u8 ICReadBuf1Byte(u16 ByteAddr); 
/*读缓存1字节*/
u8 ICReadBuf2Byte(u16 ByteAddr); 
/*读缓存2字节*/
/****************************************************************************
** 函数名称：void ICWriteBuf1(u16 ByteAddr,u8 *pData,u16 Size)
** 功    能：缓存1写操作函数
** 说    明： 
** 入口参数：ByteAddr起始地址pdata写入的数据 size数据长度
** 出口参数：写成功与否
** 全局变量：
** 作　者：黄楚雄
** 日　期：2007年8月16日
**---------------------------------------------------------------------------
** 修改人：
** 日　期：
** 内　容：
****************************************************************************/
void ICWriteBuf1(u16 ByteAddr,u8 *pData,u16 Size);/*写字符串到缓存1*/
/****************************************************************************
** 函数名称：void ICWriteBuf2(u16 ByteAddr,u8 *pData,u16 Size)
** 功    能：缓存2写操作函数
** 说    明： 
** 入口参数：ByteAddr起始地址pdata写入的数据 size数据长度
** 出口参数：写成功与否
** 全局变量：
** 作　者：黄楚雄
** 日　期：2007年8月16日
**---------------------------------------------------------------------------
** 修改人：
** 日　期：
** 内　容：
****************************************************************************/
void  ICWriteBuf2(u16 ByteAddr,u8 *pData,u16 Size);/*写字符串到缓存2*/
/*写缓存2字节*/
u8    ICBuf1CmpPage(u16 Page); /* 比较缓存1和页内容          */
u8    ICBuf2CmpPage(u16 Page); /* 比较缓存2和页内容          */
void  ICBuf1ToPage(u16 Page);  /* 缓存1写入页                */
void  ICBuf2ToPage(u16 Page);  /* 缓存2写入页                */
void  ICBuf1ToPageN(u16 Page);  /* 缓存1写入页                */
void  ICBuf2ToPageN(u16 Page);  /* 缓存2写入页                */
void  ICPageToBuf1(u16 Page);  /* 页内容至缓存1              */
void  ICPageToBuf2(u16 Page);  /* 页内容至缓存2              */
void  ICWaitReady(void);
/*等待IC卡准备好, 无超时判定*/
AT45FlashStatus   ICWaitReady_timeout(u16 timeout);  
/*等待IC卡准备好, 有超时判定*/

void  ICPageErase(u16 Page);
void  ICCommand(OpCode cmd, u16 Page,u16 ByteAddr);   /* 所有命令的执行 */
u8    ICStatus(void);               /*读IC卡状态*/

u8    ICReadByte(void);
void  ICWriteByte(u8 data);
void  ICIDR(void);                  /*读IC卡出厂ID*/
///罗显能添加2010-05-19
/*读缓存区1数据*/
void  ICReadBuf1(u16 ByteAddr,u8 *pData,u16 Size);
/*读缓存区2数据*/
void  ICReadBuf2(u16 ByteAddr,u8 *pData,u16 Size);
/*进入休眠模式*/
void  ICDeepPowerDown(void);
/*退出休眠模式*/
void  ICResumeDeepPowerDown(void);
/*扇区擦除*/
u8    ICSectorErase(u8 Sector,EraseSectorProperty EraseSector_Property);
/*块擦除*/
u8    ICBlockErase(u16 Block);
u8 AT45ICInit(void);

#endif
