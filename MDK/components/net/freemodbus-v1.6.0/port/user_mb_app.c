
/***************************************************************************

Copyright (C), 1999-2015, Tongye Tech. Co., Ltd.

* @file           user_mb_app.c
* @author         SCH-Team
* @version        V1.0.0
* @date           2015-05-21
* @brief          IO Task

History:          Revision Records

<Author>          <time>       <version >            <desc>

xiou             2015-05-21      V1.0.0           

***************************************************************************/
#define 	DSA_HMI	

#include "user_mb_app.h"
#include "mbconfig.h"
#include "MB_DataStruct.h"
#include "string.h"
#include "math.h"
#include "logicApp.h"
#include "config.h"


extern uint16_t Modbus_cnt;       //Modbus接收中断计数(用于判断屏通讯是否正常)

/*-------------------------------------------------------------------------------
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
-------------------------Slave mode use these variables--------------------------
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
//Slave mode:DiscreteInputs variables
USHORT usSDiscInStart = S_DISCRETE_INPUT_START;

#if S_DISCRETE_INPUT_NDISCRETES % 8
UCHAR ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8+1];
#else
UCHAR ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8];
#endif

//Slave mode:Coils variables
USHORT usSCoilStart = S_COIL_START;

#if S_COIL_NCOILS % 8
UCHAR ucSCoilBuf[S_COIL_NCOILS/8+1];
#else
UCHAR ucSCoilBuf[S_COIL_NCOILS/8];
#endif

//Slave mode:InputRegister variables
USHORT usSRegInStart = S_REG_INPUT_START;
USHORT usSRegInBuf[S_REG_INPUT_NREGS];
//Slave mode:HoldingRegister variables
USHORT usSRegHoldStart = S_REG_HOLDING_START;
USHORT usSRegHoldBuf[S_REG_HOLDING_NREGS];
#endif

/*-------------------------------------------------------------------------------
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
-------------------------Master mode use these variables-------------------------
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

//Master mode:DiscreteInputs variables
USHORT usMDiscInStart = M_DISCRETE_INPUT_START;

#if M_DISCRETE_INPUT_NDISCRETES % 8
UCHAR ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8+1];
#else
UCHAR ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8];
#endif

//Master mode:Coils variables
USHORT usMCoilStart = M_COIL_START;

#if M_COIL_NCOILS % 8
UCHAR ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8+1];
#else
UCHAR ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8];
#endif

//Master mode:InputRegister variables
USHORT usMRegInStart = M_REG_INPUT_START;
USHORT usMRegInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];
//Master mode:HoldingRegister variables
USHORT usMRegHoldStart = M_REG_HOLDING_START;
USHORT usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];
#endif


//USHORT usSRegTestBuf[S_REG_HOLDING_NREGS];
USHORT HmiRegAddress;

//******************************modbus周期执行函数*********************************
//函数定义: void Bsp_Modbus_Cycle(void)
//描    述：modbus周期函数,负责主机调度以及数据更新
//入口参数：无
//出口参数：无
//备    注：无
//**********************************************************************************
void Bsp_Modbus_Cycle(void)
{
	static uint16_t timeDelay = 0;
	//static uint16_t timeCnt = 0;
	static uint8_t VersionInit = 0;
	//uint8_t	i;
	
	//维护周期
	if((++timeDelay) >= 65000)
	{
		timeDelay = 0;
	}
	
	//周期执行主机函数
#if MB_MASTER_RTU_ENABLED > 0	|| MB_MASTER_ASCII_ENABLED > 0
	if(timeDelay % 20 == 0)	
	{
		/* Test modbus master*/
		//eMBMasterReqWriteMultipleHoldingRegister(1,2,2,usSRegTestBuf,RT_WAITING_FOREVER);
	}
	else if((timeDelay % 20) == 10)
	{
		eMBMasterReqReadHoldingRegister(1,0,10,RT_WAITING_FOREVER);
	}
#endif
	
	
	//周期更新数据
	if(timeDelay % 10 == 0)
	{		
//		usSRegTestBuf[0] = (timeCnt) & 0xffff;
//		usSRegTestBuf[1] = (timeCnt++) & 0xffff;
	//周期更新从机数据
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
		//rt_memcpy(usSRegHoldBuf, usSRegTestBuf, S_REG_HOLDING_NREGS * 2);
			
		if(Modbus_cnt > 0)
		{
			Modbus_cnt = 0;	
			MB_LGA.MB_SYS_INFO.HmiFault = 0;
		}
		else
		{
			Modbus_cnt = 0;
			MB_LGA.MB_SYS_INFO.HmiFault = 1;  //通讯异常	
			return;
		}		
		
		//--------------------------------------------------------------
		// DSP向触摸屏输出
		//--------------------------------------------------------------		
		PDHMI(Reg)->mbStatus = HMI.mbStatus;
		
		PDHMI(Reg)->mbRdPara = HMI.mbRdPara;
		
		PDHMI(Reg)->Modue1_Para = HMI.Modue1_Para;
		PDHMI(Reg)->Modue2_Para = HMI.Modue2_Para;
		PDHMI(Reg)->Modue3_Para = HMI.Modue3_Para;
		PDHMI(Reg)->Modue4_Para = HMI.Modue4_Para;
		PDHMI(Reg)->Modue5_Para = HMI.Modue5_Para;
		if (VersionInit == 0)
		{
			PDHMI(Reg)->version = HMI.version;
		}
		//--------------------------------------------------------------
		// 触摸屏向Monitor输出
		//--------------------------------------------------------------
//		for (i=0; i<HMI_ADDR_W_NUM; i++)
//		{
//			if (HMI.HMI_WAddr_flag[i])
//				break;
//		}
//		
//		if (i==ADDR_CHARGE)
//		{
//			HMI.CtrCmd.ChargeCmd = PDHMI(Reg)->CtrCmd.ChargeCmd;
//		}
//		
//		else if (i==ADDR_DEV_TEST)
//		{
//			HMI.CtrCmd.DevTestCmd = PDHMI(Reg)->CtrCmd.DevTestCmd;
//		}
//		
//		else if (i==ADDR_SLEEP)
//		{
//			HMI.CtrCmd.SleepCmd = PDHMI(Reg)->CtrCmd.SleepCmd;
//		}
//		
//		else if (i==ADDR_RESTART)
//		{
//			HMI.CtrCmd.RestartCmd = PDHMI(Reg)->CtrCmd.RestartCmd;
//		}
//		
//		else if (i==ADDR_RESTORE)
//		{
//			HMI.CtrCmd.RestoreParCmd = PDHMI(Reg)->CtrCmd.RestoreParCmd;
//		}
//		
//		else if (i==ADDR_W_MB_PARA)
//		{
//			HMI.mbWrPara = PDHMI(Reg)->mbWrPara;
//		}
//			
//		else if (i==ADDR_W_CB1_PARA)	
//		{
//			HMI.Modue1_WPara = PDHMI(Reg)->Modue1_WPara;
//		}
//			
//		else if (i==ADDR_W_CB2_PARA)	
//		{
//			HMI.Modue2_WPara = PDHMI(Reg)->Modue2_WPara;
//		}
//			
//		else if (i==ADDR_W_CB3_PARA)	
//		{
//			HMI.Modue3_WPara = PDHMI(Reg)->Modue3_WPara;
//		}
//			
//		else if (i==ADDR_W_CB4_PARA)	
//		{
//			HMI.Modue4_WPara = PDHMI(Reg)->Modue4_WPara;
//		}
//			
//		else if (i==ADDR_W_CB5_PARA)	
//		{
//			HMI.Modue5_WPara = PDHMI(Reg)->Modue5_WPara;
//		}
//			
//		else if (i==ADDR_W_MB_RTC)	
//		{
//			HMI.Time = PDHMI(Reg)->Time;
//		}
			
#endif
	}
}

void RegAddressGet(USHORT Address)
{
	HmiRegAddress = Address - 1;
	switch(HmiRegAddress){
			case HMI_ADDR_CHARGE_EN:
				HMI.HMI_WAddr_flag[ADDR_CHARGE_EN] = true;
				HMI.CtrCmd.ChargeEn = PDHMI(Reg)->CtrCmd.ChargeEn;
				break;
			
			case HMI_ADDR_CHARGE_CMD :
				HMI.HMI_WAddr_flag[ADDR_CHARGE_CMD] = true;
				HMI.CtrCmd.ChargeCmd = PDHMI(Reg)->CtrCmd.ChargeCmd;
				break;
			
			case HMI_ADDR_DEV_TEST :
				HMI.HMI_WAddr_flag[ADDR_DEV_TEST] = true;
				HMI.CtrCmd.DevTestCmd = PDHMI(Reg)->CtrCmd.DevTestCmd;
				break;
			
			case HMI_ADDR_SLEEP :
				HMI.HMI_WAddr_flag[ADDR_SLEEP] = true;
				HMI.CtrCmd.SleepCmd = PDHMI(Reg)->CtrCmd.SleepCmd;
				break;
			
			case HMI_ADDR_RESTART :
				HMI.HMI_WAddr_flag[ADDR_RESTART] = true;
				HMI.CtrCmd.RestartCmd = PDHMI(Reg)->CtrCmd.RestartCmd;
				break;
			
			case HMI_ADDR_RESTORE :
				HMI.HMI_WAddr_flag[ADDR_RESTORE] = true;
				HMI.CtrCmd.RestoreParCmd = PDHMI(Reg)->CtrCmd.RestoreParCmd;
				break;
			
			case HMI_ADDR_W_MB_PARA :
				HMI.HMI_WAddr_flag[ADDR_W_MB_PARA] = true;
				HMI.mbWrPara = PDHMI(Reg)->mbWrPara;
				break;
			
			case HMI_ADDR_W_CB1_PARA :
				HMI.HMI_WAddr_flag[ADDR_W_CB1_PARA] = true;
				HMI.Modue1_WPara = PDHMI(Reg)->Modue1_WPara;
				break;
			
			case HMI_ADDR_W_CB2_PARA :
				HMI.HMI_WAddr_flag[ADDR_W_CB2_PARA] = true;
				HMI.Modue2_WPara = PDHMI(Reg)->Modue2_WPara;
				break;
			
			case HMI_ADDR_W_CB3_PARA :
				HMI.HMI_WAddr_flag[ADDR_W_CB3_PARA] = true;
				HMI.Modue3_WPara = PDHMI(Reg)->Modue3_WPara;
				break;
			
			case HMI_ADDR_W_CB4_PARA :
				HMI.HMI_WAddr_flag[ADDR_W_CB4_PARA] = true;
				HMI.Modue4_WPara = PDHMI(Reg)->Modue4_WPara;
				break;
			
			case HMI_ADDR_W_CB5_PARA :
				HMI.HMI_WAddr_flag[ADDR_W_CB5_PARA] = true;
				HMI.Modue5_WPara = PDHMI(Reg)->Modue5_WPara;
				break;
			
			case HMI_ADDR_W_MB_RTC :
				HMI.HMI_WAddr_flag[ADDR_W_MB_RTC] = true;
				HMI.Time = PDHMI(Reg)->Time;
				break;
			
			default:
				break;
		}
}

//******************************输入寄存器回调函数**********************************
//函数定义: eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
//描    述：输入寄存器相关的功能（读、连续读）
//入口参数：pucRegBuffer : 回调函数将Modbus寄存器的当前值写入的缓冲区
//			usAddress    : 寄存器的起始地址，输入寄存器的地址范围是1-65535。
//			usNRegs      : 寄存器数量
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2010-10-31    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;
	USHORT *        pusRegInputBuf;
	USHORT           REG_INPUT_START;
	USHORT           REG_INPUT_NREGS;
	USHORT           usRegInStart;

	//Determine the master or slave
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
	if (xMBMasterGetCBRunInMasterMode())
	{
		pusRegInputBuf = usMRegInBuf[ucMBMasterGetDestAddress()];
		REG_INPUT_START = M_REG_INPUT_START;
		REG_INPUT_NREGS = M_REG_INPUT_NREGS;
		usRegInStart = usMRegInStart;
	}
#endif
	
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
	//pusRegInputBuf = usSRegHoldBuf;
	pusRegInputBuf = usSRegInBuf;
	REG_INPUT_START = S_REG_INPUT_START;
	REG_INPUT_NREGS = S_REG_INPUT_NREGS;
	usRegInStart = usSRegInStart;
#endif

	if( ( usAddress >= REG_INPUT_START )
		&& ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
	{
		iRegIndex = ( int )( usAddress - usRegInStart );
		while( usNRegs > 0 )
		{
			//Determine the master or slave
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
			if (xMBMasterGetCBRunInMasterMode())
			{
				pusRegInputBuf[iRegIndex] = *pucRegBuffer++ << 8;
				pusRegInputBuf[iRegIndex] |= *pucRegBuffer++;
			}
			else
#endif
			{
				*pucRegBuffer++ = ( unsigned char )( pusRegInputBuf[iRegIndex] >> 8 );
				*pucRegBuffer++ = ( unsigned char )( pusRegInputBuf[iRegIndex] & 0xFF );
			}
			iRegIndex++;
			usNRegs--;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}
//******************************保持寄存器回调函数**********************************
//函数定义: eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
//描    述：保持寄存器相关的功能（读、连续读、写、连续写）
//入口参数：pucRegBuffer : 如果需要更新用户寄存器数值，这个缓冲区必须指向新的寄存器数值。
//                         如果协议栈想知道当前的数值，回调函数必须将当前值写入这个缓冲区
//			usAddress    : 寄存器的起始地址。
//			usNRegs      : 寄存器数量
//          eMode        : 如果该参数为eMBRegisterMode::MB_REG_WRITE，用户的应用数值将从pucRegBuffer中得到更新。
//                         如果该参数为eMBRegisterMode::MB_REG_READ，用户需要将当前的应用数据存储在pucRegBuffer中
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2010-10-31    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;
	USHORT *        pusRegHoldingBuf;
	USHORT           REG_HOLDING_START;
	USHORT           REG_HOLDING_NREGS;
	USHORT           usRegHoldStart;

	//Determine the master or slave
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
	if (xMBMasterGetCBRunInMasterMode())
	{
		pusRegHoldingBuf = usMRegHoldBuf[ucMBMasterGetDestAddress()];
		REG_HOLDING_START = M_REG_HOLDING_START;
		REG_HOLDING_NREGS = M_REG_HOLDING_NREGS;
		usRegHoldStart = usMRegHoldStart;
		//If mode is read,the master will wirte the received date to bufffer.
		eMode = MB_REG_WRITE;
	}
#endif
	
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
	//pusRegHoldingBuf = usSRegHoldBuf;
	pusRegHoldingBuf = (USHORT *)PDHMI(Reg);
	//HmiRegAddress = usAddress - 1;
	//RegAddressGet (usAddress);
	REG_HOLDING_START = S_REG_HOLDING_START;
	REG_HOLDING_NREGS = S_REG_HOLDING_NREGS;
	usRegHoldStart = usSRegHoldStart;
#endif

	if( ( usAddress >= REG_HOLDING_START ) &&
		( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
	{
		iRegIndex = ( int )( usAddress - usRegHoldStart );
		switch ( eMode )
		{
			/* Pass current register values to the protocol stack. */
		case MB_REG_READ:
			while( usNRegs > 0 )
			{
				*pucRegBuffer++ = ( unsigned char )( pusRegHoldingBuf[iRegIndex] >> 8 );
				*pucRegBuffer++ = ( unsigned char )( pusRegHoldingBuf[iRegIndex] & 0xFF );
				iRegIndex++;
				usNRegs--;
			}
			break;

			/* Update current register values with new values from the
			* protocol stack. */
		case MB_REG_WRITE:
			while( usNRegs > 0 )
			{
				pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
				pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
				iRegIndex++;
				usNRegs--;
			}
			break;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	RegAddressGet (usAddress);
	return eStatus;
}
//****************************线圈状态寄存器回调函数********************************
//函数定义: eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
//描    述：线圈状态寄存器相关的功能（读、连续读、写、连续写）
//入口参数：pucRegBuffer : 位组成一个字节，起始寄存器对应的位处于该字节pucRegBuffer的最低位LSB。
//                         如果回调函数要写这个缓冲区，没有用到的线圈（例如不是8个一组的线圈状态）对应的位的数值必须设置位0。
//			usAddress    : 第一个线圈地址。
//			usNCoils     : 请求的线圈个数
//          eMode        ；如果该参数为eMBRegisterMode::MB_REG_WRITE，用户的应用数值将从pucRegBuffer中得到更新。
//                         如果该参数为eMBRegisterMode::MB_REG_READ，用户需要将当前的应用数据存储在pucRegBuffer中
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2010-10-31    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex , iRegBitIndex , iNReg;
	UCHAR *         pucCoilBuf;
	USHORT           COIL_START;
	UCHAR           COIL_NCOILS;
	USHORT           usCoilStart;
	iNReg =  usNCoils / 8 + 1;        //占用寄存器数量


	//Determine the master or slave
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
	if (xMBMasterGetCBRunInMasterMode())
	{
		pucCoilBuf = ucMCoilBuf[ucMBMasterGetDestAddress()];
		COIL_START = M_COIL_START;
		COIL_NCOILS = M_COIL_NCOILS;
		usCoilStart = usMCoilStart;
		//If mode is read,the master will wirte the received date to bufffer.
		eMode = MB_REG_WRITE;
	}
#endif
	
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
	pucCoilBuf = ucSCoilBuf;
	COIL_START = S_COIL_START;
	COIL_NCOILS = S_COIL_NCOILS;
	usCoilStart = usSCoilStart;
#endif

	if( ( usAddress >= COIL_START ) &&
		( usAddress + usNCoils <= COIL_START + COIL_NCOILS ) )
	{
		iRegIndex    = ( int )( usAddress - usCoilStart ) / 8 ;    //每个寄存器存8个
		iRegBitIndex = ( int )( usAddress - usCoilStart ) % 8 ;	   //相对于寄存器内部的位地址
		switch ( eMode )
		{
			/* Pass current coil values to the protocol stack. */
		case MB_REG_READ:
			while( iNReg > 0 )
			{
				*pucRegBuffer++ = xMBUtilGetBits(&pucCoilBuf[iRegIndex++] , iRegBitIndex , 8);
				iNReg --;
			}
			pucRegBuffer --;
			usNCoils = usNCoils % 8;                        //余下的线圈数	
			*pucRegBuffer = *pucRegBuffer <<(8 - usNCoils); //高位补零
			*pucRegBuffer = *pucRegBuffer >>(8 - usNCoils);
			break;

			/* Update current coil values with new values from the
			* protocol stack. */
		case MB_REG_WRITE:
			while(iNReg > 1)									 //最后面余下来的数单独算
			{
				xMBUtilSetBits(&pucCoilBuf[iRegIndex++] , iRegBitIndex  , 8 , *pucRegBuffer++);
				iNReg--;
			}
			usNCoils = usNCoils % 8;                            //余下的线圈数
			if (usNCoils != 0)                                  //xMBUtilSetBits方法 在操作位数量为0时存在bug
			{
				xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils,
					*pucRegBuffer++);
			}
			break;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}
//****************************离散输入寄存器回调函数********************************
//函数定义: eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
//描    述：离散输入寄存器相关的功能（读、连续读）
//入口参数：pucRegBuffer : 用当前的线圈数据更新这个寄存器，起始寄存器对应的位处于该字节pucRegBuffer的最低位LSB。
//                         如果回调函数要写这个缓冲区，没有用到的线圈（例如不是8个一组的线圈状态）对应的位的数值必须设置为0。
//			usAddress    : 离散输入的起始地址
//			usNDiscrete  : 离散输入点数量
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2010-10-31    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex , iRegBitIndex , iNReg;
	UCHAR *         pucDiscreteInputBuf;
	USHORT           DISCRETE_INPUT_START;
	USHORT           DISCRETE_INPUT_NDISCRETES;
	USHORT           usDiscreteInputStart;
	iNReg =  usNDiscrete / 8 + 1;        //占用寄存器数量

	//Determine the master or slave
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
	if (xMBMasterGetCBRunInMasterMode())
	{
		pucDiscreteInputBuf = ucMDiscInBuf[ucMBMasterGetDestAddress()];
		DISCRETE_INPUT_START = M_DISCRETE_INPUT_START;
		DISCRETE_INPUT_NDISCRETES = M_DISCRETE_INPUT_NDISCRETES;
		usDiscreteInputStart = usMDiscInStart;
	}
#endif
	
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
	pucDiscreteInputBuf = ucSDiscInBuf;
	DISCRETE_INPUT_START = S_DISCRETE_INPUT_START;
	DISCRETE_INPUT_NDISCRETES = S_DISCRETE_INPUT_NDISCRETES;
	usDiscreteInputStart = usSDiscInStart;
#endif

	if( ( usAddress >= DISCRETE_INPUT_START )
		&& ( usAddress + usNDiscrete <= DISCRETE_INPUT_START + DISCRETE_INPUT_NDISCRETES ) )
	{
		iRegIndex    = ( int )( usAddress - usDiscreteInputStart ) / 8 ;    //每个寄存器存8个
		iRegBitIndex = ( int )( usAddress - usDiscreteInputStart ) % 8 ;	   //相对于寄存器内部的位地址

		//Determine the master or slave
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
		if (xMBMasterGetCBRunInMasterMode())
		{
			/* Update current coil values with new values from the
			* protocol stack. */
			while(iNReg > 1)									 //最后面余下来的数单独算
			{
				xMBUtilSetBits(&pucDiscreteInputBuf[iRegIndex++] , iRegBitIndex  , 8 , *pucRegBuffer++);
				iNReg--;
			}
			usNDiscrete = usNDiscrete % 8;                        //余下的线圈数
			if (usNDiscrete != 0)                                 //xMBUtilSetBits方法 在操作位数量为0时存在bug
			{
				xMBUtilSetBits(&pucDiscreteInputBuf[iRegIndex++], iRegBitIndex,
					usNDiscrete, *pucRegBuffer++);
			}
		}
		else
#endif
		{
			while( iNReg > 0 )
			{
				*pucRegBuffer++ = xMBUtilGetBits(&pucDiscreteInputBuf[iRegIndex++] , iRegBitIndex , 8);
				iNReg --;
			}
			pucRegBuffer --;
			usNDiscrete = usNDiscrete % 8;                     //余下的线圈数
			*pucRegBuffer = *pucRegBuffer <<(8 - usNDiscrete); //高位补零
			*pucRegBuffer = *pucRegBuffer >>(8 - usNDiscrete);
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}


/*******************************************************************************
* Function Name  : void preg(u8 addr,u8 num)
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void preg(u8 addr,u8 num)
{
	u8 i = 0;

	for(i=1 ; i <= num ;i++)
	{
#if (USE_FREE_MODBUS)
#if MB_SLAVE_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
		rt_kprintf("%2X |",usSRegHoldBuf[addr++]);
#else
		rt_kprintf("%2X |",usMRegHoldBuf[1][addr++]);
#endif
#endif

		if(i%8 == 0)
			rt_kprintf("	");
		if(i%16 == 0)
			rt_kprintf("\r\n");
	}

}

#include "finsh.h"
FINSH_FUNCTION_EXPORT(preg, preg(addr,num))

