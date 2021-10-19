/*
 * motor.h
 *
 *  Created on: 2021年7月28日
 *      Author: liuhuiyuan
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include<stdlib.h>
#include<msp430.h>
#include<string.h>
#include<stdio.h>
#define Zx_Motor_FRAME_HEADER1  0XFA
#define Zx_Motor_FRAME_HEADER2  0XAF
#define Zx_Motor_MOVE_ANGLE     0X01
#define Zx_Motor_LED            0X04
#define Zx_Motor_READ_ANGLE     0X02
#define Zx_Motor_ID_WRITE       0XCD
#define Zx_Motor_SET_OFFSET     0XD2
#define Zx_Motor_READ_OFFSET    0XD4
#define Zx_Motor_VERSION        0X01
#define Zx_Motor_FRAME_END      0XED
#define Zx_Motor_RUNS           0XFD
#define Zx_Motor_RUNN           0XFE

void delay(unsigned int ms);         //延时函数
void USCIA0_Init(void);              //串口0初始化
void USCIA1_Init(void);              //串口1初始化
void  Zx_MoterMove(unsigned char position);  //舵机函数 0~180;90为中间值
unsigned char Zx_MotorCheckSum(unsigned char buf[]);
void Zx_MotorRunS(unsigned char id,long rpm);
//电机正转  其中id为电机id号 ，rpm为给定电机转数速度 0--2000
void Zx_MotorRunN(unsigned char id,long rpm);
//电机反转  其中id为电机id号 ，rpm为给定电机转数速度 0--2000
long Speedcontrol(long speed,long schange);  //后轮同速控制
void Speedcontrol_D(long lefts,long rights);//后轮差速控制方向
long Stopcontrol();  //后轮同速停止
int Degreecontrol(int degree,int dchange);//前轮角度控制
int Degreere();//前轮角度归零

#endif /* MOTOR_H_ */
