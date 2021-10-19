/*
 * motor.c
 *
 *  Created on: 2021年7月28日
 *      Author: liuhuiyuan
 */


#include<motor.h>
#define GET_LOW_BYTE(A)  ((unsigned int)(A))
#define GET_HIGH_BYTE(A) ((unsigned int)((A) >> 8))

void delay(unsigned int ms)        //延时函数
{
    unsigned int i=0;
    unsigned int j=0;
    for(i=0;i<ms;i++) for(j=0;j<240;j++);
}


void USCIA0_Init(void)            //串口0初始化
{
    P3SEL    |=  BIT4+BIT3 ;                        // P3.4,3 = USCI_A0 TXD/RXD
    UCA0CTL1 |=  UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |=  UCSSEL_2;                     // ACLK
    UCA0BR0   =  0x09;                         // 32768Hz  波特率为11520
    UCA0BR1   =  0x00;                         // 32768Hz 11520
    UCA0MCTL |=  UCBRS_3 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST;                      // **Initialize USCI state machine**
    UCA0IE   |=  UCRXIE;                       // Enable USCI_A1 RX interrupt 使能中断
}

void USCIA1_Init(void)            //uart1串口初始化
{
    P4SEL    |=  BIT4+BIT5 ;                        // P4.5,6 = USCI_A1 TXD/RXD
    UCA1CTL1 |=  UCSWRST;                      // **Put state machine in reset**
    UCA1CTL1 |=  UCSSEL_2;                     // ACLK
    UCA1BR0   =  0x09;                         // 32768Hz  波特率为115200
    UCA1BR1   =  0x00;                             // 32768Hz  波特率为115200*/
/*     UCA1CTL1 |=  UCSSEL_1;                     // ACLK
    UCA1BR0   =  0x03;                         //   波特率为9600
    UCA1BR1   =  0x00;*/
    UCA1MCTL |=  UCBRS_3 + UCBRF_0;           // Modulation UCBRSx=1, UCBRFx=0
    UCA1CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    UCA1IE   |=  UCRXIE;                         // Enable USCI_A1 RX interrupt 使能中断
}

void  Zx_MoterMove(unsigned char position)    //舵机函数  position 取值范围（0-180）
{
    TA1CCR0 = 100;                    //输出PWM波周期
    TA1CCR1 = (22+position*0.28888);   // 占空比
    P2DIR |=BIT0;
    P2SEL |=BIT0;
}

unsigned char Zx_MotorCheckSum(unsigned char buf[])  //
{
    unsigned char i;
    unsigned long sum=0;
    for(i=2;i<8;i++)
    {
        sum +=buf[i];
    }
    if(sum>255)sum&=0x00FF;
  return sum;
}

void Zx_MotorRunS(unsigned char id,long rpm)//电机正转  其中id为电机id号 ，rpm为给定电机转数速度 0--2000
{
    unsigned char i;
    unsigned char  buf[10];
    buf[0]=Zx_Motor_FRAME_HEADER1;
    buf[1]=Zx_Motor_FRAME_HEADER2;
    buf[2]=id;
    buf[3]=0x01  ;
    buf[4]=Zx_Motor_RUNS;
    buf[5]=0x00;
    buf[6]=GET_HIGH_BYTE(rpm);
    buf[7]=GET_LOW_BYTE(rpm);
    buf[8]=Zx_MotorCheckSum(buf);
    buf[9]=Zx_Motor_FRAME_END;
    for(i=0;i<10;i++){while(!(UCA0IFG));UCA0TXBUF=buf[i];};//将指令发送出去
}

void Zx_MotorRunN(unsigned char id,long rpm) //电机反转  其中id为电机id号 ，rpm为给定电机转数速度 0--2000
{
    unsigned char i;
    unsigned char  buf[10];
    buf[0]=Zx_Motor_FRAME_HEADER1;
    buf[1]=Zx_Motor_FRAME_HEADER2;
    buf[2]=id;
    buf[3]=0x01;
    buf[4]=Zx_Motor_RUNN;
    buf[5]=0x00;
    buf[6]=GET_HIGH_BYTE(rpm);
    buf[7]=GET_LOW_BYTE(rpm);
    buf[8]=Zx_MotorCheckSum(buf);
    buf[9]= Zx_Motor_FRAME_END;
    for(i=0;i<10;i++){while(!(UCA0IFG));UCA0TXBUF=buf[i];}//将指令发送出去
}

long Speedcontrol(long speed,long schange)  //后轮同速控制
{
    long fs;
    fs=speed+schange;
    if(fs>=0 && fs<=65500){Zx_MotorRunS(2,fs);Zx_MotorRunN(1,fs);}
    else if(fs<0 && fs>=-65500){Zx_MotorRunS(1,-fs);Zx_MotorRunN(2,-fs);}
    else if(fs>65500){fs=65500;Zx_MotorRunS(2,fs);Zx_MotorRunN(1,fs);}
    else{fs=-65500;Zx_MotorRunS(1,-fs);Zx_MotorRunN(2,-fs);}
    return fs;
}


void Speedcontrol_D(long lefts,long rights)  //后轮差速控制转向
{
    Zx_MotorRunS(2,rights);
    Zx_MotorRunN(1,lefts);
}

long Stopcontrol()  //后轮同速停止
{
    Zx_MotorRunS(1,0);
    Zx_MotorRunN(2,0);
    return 0;
}

int Degreecontrol(int degree,int dchange)//前轮角度控制
{
    int fd;
    fd=degree+dchange;
    if(fd>180)fd=180;
    else if(fd<0)fd=0;
    Zx_MoterMove(fd);
    return fd;
}

int Degreere()//前轮角度归零
{
    Zx_MoterMove(90);
    return 90;
}







