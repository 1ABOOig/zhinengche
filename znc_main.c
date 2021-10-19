#include <motor.h>

//int  Position=90;
//long Speed=50;
unsigned char Data[7];
unsigned char DataR[4];
unsigned char DataT[4];
unsigned char Dflag=2;
unsigned char Dchoice=0;
unsigned char Dnum1=0;
unsigned char Dnum=0;
unsigned char DnumT=0;
unsigned char DnumR=0;
unsigned char Flag;
int Theta,Rho;

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;      //关闭看门狗
    P6DIR |= BIT5;                               //数据可接收标识
    P2DIR |= BIT0;                               //P2.0输出PWM波控制舵机
    TA1CCTL1 = OUTMOD_7;                         //将输出模式选择为模式7
    TA1CTL = TASSEL_1 + MC_1 +TACLR ;            //选择ACLK时钟源，timerA计数模式选择up模式，计数到TA0CCR0，重置timerA计数器
    USCIA0_Init();                  //串口0初始化
    USCIA1_Init();                  //串口1初始化
    _EINT();// 开启程序的总中断
    Degreere();                     //舵机归位
    Stopcontrol();                  //后轮停止
    P6OUT |= BIT5;                  //允许数据传送
    while(1)
    {
        if(Dflag==1)                                                        //指示符置1时，开始数据处理
        {
            for(Dnum1=0;Dnum1<7;Dnum1++)                                       //按位处理7位数据  ***a***
            {
                if(Data[Dnum1]=='a'){Dchoice=1;DnumT=Dnum1;Dnum1++;}           //读取到a表示Rho数据结束，数据选择指示符置1
                if(Dchoice==0){DataT[Dnum1]=Data[Dnum1];}                     //指示符置0时，从Data收录Theta数据
                else if(Dchoice==1){DataR[DnumR]=Data[Dnum1];DnumR++;}       //指示符置1时，从Data收录Rho数据
            }
            DataT[DnumT]='\0';Theta=atoi(DataT);          //处理Theta标志
            DataR[DnumR]='\0';Rho=atoi(DataR);            //处理Rho数据
            if(Theta==200 && Rho==200)Stopcontrol();      //后轮停止
            else{
            Zx_MoterMove(Theta);                          //前轮舵机控制
            Speedcontrol_D(20+Rho,20-Rho);          //后轮差速控制
            Dnum=0;DnumR=0;DnumT=0;Dchoice=0;Dflag=2;          //各标识符复位
            P6OUT |= BIT5;}                            //允许下次数据传送
        }
    }
}
#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR()
{
    switch(_even_in_range(UCA1IV,4))
    {
    case 0:break;                             // Vector 0 - No interrupt
        case 2:                               // Vector 2 - RXIFG
            while (!(UCA1IFG));                                 //等待完成接收(数据接收格式为S***a***D)
//            UCA1TXBUF=UCA1RXBUF;
            if(UCA1RXBUF=='D')Dflag=1;                          //读取到D表示数据结束，指示符置1
            else if(Dflag==0){Data[Dnum]=UCA1RXBUF;Dnum++;}     //指示符置0时，从读取寄存器收录数据
            else if(UCA1RXBUF=='S'){Dflag=0;P6OUT &= ~BIT5;}    //读取到S表示数据开始，指示符置0
            break;
     case 4:break;                            // Vector 4 - TXIFG
     default: break;
    }
}

#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR()
{
    switch(_even_in_range(UCA0IV,4))
    {
    case 0:break;                             // Vector 0 - No interrupt
        case 2:                               // Vector 2 - RXIFG
            while (!(UCA0IFG));       // USCI_A1 TX buffer ready?
            while(!(UCA0RXBUF==0xAB || UCA0RXBUF==0xAC))
            break;
     case 4:break;                            // Vector 4 - TXIFG
     default: break;
    }
}
