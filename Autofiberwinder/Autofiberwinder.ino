// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       Autofiberwinder.ino
    Created:	2018/7/12 17:19:19
    Author:     LIUNIANSKYLAKEX\liunian
*/

// Define User Types below here or use a .h file

#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <math.h>




int pin_Joystick1_X = A0; //摇杆控制器X引脚
int pin_Joystick1_Y = A1; //摇杆控制器Y引脚
int pin_StepperX_PUL = 8;  //StepperX_滑台电机_PUL_脉冲引脚
int pin_StepperX_DIR = 9;	//StepperX_滑台电机_PUL_方向引脚
int pin_StepperY_PUL = 10;  //StepperY——绕线电机_PUL_脉冲引脚
int pin_StepperY_DIR = 11;	//StepperY——绕线电机_PUL_方向引脚
int pin_Switch1_switch = 12;  //限位开关左
int pin_Switch2_switch = 13;  //限位开关右
int X = pin_StepperX_PUL;	//X轴（滑台电机）脉冲引脚
int Y = pin_StepperY_PUL;	//X轴（绕线电机）脉冲引脚
int posX, posY, tgtX[100], tgtY[100], kX, kY;
int StepperX_multiplier = 1;
int StepperY_multiplier = 1;
int StepperX_pulsePerRotation = 400; //滑台电机周转脉冲数;
int StepperY_pulsePerRotation = 400; //主轴电机周转脉冲数;
int Globalinterval = 1000; //全局时间间隔 微秒


void setup()
{
	pinMode(pin_Joystick1_X, INPUT);
	pinMode(pin_Joystick1_Y, INPUT);
	pinMode(pin_StepperX_PUL, OUTPUT);
	pinMode(pin_StepperX_DIR, OUTPUT);
	pinMode(pin_StepperY_PUL, OUTPUT);
	pinMode(pin_StepperY_DIR, OUTPUT);

	pinMode(pin_Switch1_switch, INPUT);
	pinMode(pin_Switch2_switch, INPUT);

	Serial.begin(9600);

}

// Add the main program code into the continuous loop() function
void loop(){
	manualControl();
}

int linearto(int tgtx,int tgty, int dtime) {	//线性插补移动到tgtx,tgty,用时dtime（微秒） 
	bool kx; //X轴运动方向标记
	bool ky; //y轴运动方向标记
	int F;	//直线插补偏差
	int sigma = tgtx + tgty - posX - posY; //总偏差量-总脉冲数
	float distance = sqrt((tgtx-posX)^2+(tgty-posY)^2);
	bool dir;
	int localInterval = dtime / sigma;

	if (tgtx >= posX & tgty >= posY) //第一象限直线
	{
		kx = 1;
		ky = 1;
	}
	if (tgtx < posX & tgty >= posY) //第二象限
	{
		kx = 0;
		ky = 1;
	}
	if (tgtx < posX & tgty < posY) //第三象限
	{
		kx = 0;
		ky = 0;
	}
	if (tgtx >= posX & tgty < posY) //第四象限
	{
		kx = 0;
		ky = 1;
	}

	while (sigma != 0)
	{

		if (F >= 0)
		{
			drive(X, kx, localInterval);
			F = F - tgty;
		}
		else
		{
			drive(Y, ky, localInterval);
			F = F + tgtx;
		}
		sigma = tgtx + tgty - posX - posY;
	}
	return(1);
}

void drive(int pin_PUL, bool dir, int periodmicros)   //按dir方向驱动pin_PUL
{
	if (pin_PUL = pin_StepperX_PUL)
	{
		digitalWrite(pin_StepperX_DIR, dir);                     //滑台低电平（0，low）向左
		pulseOnce(pin_PUL, periodmicros);
		if (!dir)
		{														 //dir=0滑台向左
			posX = posX - 1;
		}
		else
		{
			posX = posX + 1;
		}
	}

	if (pin_PUL = pin_StepperY_PUL)
	{
		digitalWrite(pin_StepperY_DIR, dir);                     //主轴低电平（0，low）向后
		pulseOnce(pin_PUL, periodmicros);
		if (!dir)
		{														//dir=0主轴向后退纱
			posY = posY - 1;
		}
		else
		{
			posY = posY + 1;
		}
	}
}

void ZeroX() {
	Serial.println("Finding Zero point of X axis, please stand by...");
	digitalWrite(pin_StepperX_DIR, LOW); //滑台向左
	delay(100);
	while (!digitalRead(pin_Switch1_switch)) { //开关未被触发时
		pulseOnce(pin_StepperX_PUL, 500);
	}

	delay(1000);
	digitalWrite(pin_StepperX_DIR, HIGH);
	for (int i = 0; i < StepperX_pulsePerRotation; i++) {
		pulseOnce(pin_StepperX_PUL, 500);
	}
	posX = 0;
	Serial.println("Zero point of X axis found.");
}
void ZeroY() { posY = 0; }

void pulseOnce(int pin, int intevalus)  //(pin,periodmicros)向发送单词方波脉冲 脉冲宽度periodmicros，单位微秒
{
	digitalWrite(pin, HIGH);
	delayMicroseconds(intevalus / 2);
	digitalWrite(pin, LOW);
	delayMicroseconds(intevalus / 2);
}

int sgn(int x)
{
	if (x == 0) //当x==0时，函数返回0
		return 0;
	else if (x > 0) //当x>0时，函数返回1
		return 1;
	else  //当x<0时，函数返回－1
		return -1;
}

void manualControl() 
{
	int offsetX_Joystick1 = map(analogRead(pin_Joystick1_X), 0, 1024, -100, 100);
	int offsetY_Joystick1 = map(analogRead(pin_Joystick1_Y), 0, 1024, -100, 100);
	int offset = sqrt(pow(offsetX_Joystick1,2) + pow(offsetY_Joystick1, 2));
	Serial.println(offset);
	
	
	if (offset>15) 
	{
		int interval = 3000;
		if (offset > 100) {
			digitalWrite(pin_StepperX_DIR, LOW);
			pulseOnce(pin_StepperX_PUL, interval);
		}
		if (offset < -100) {
			digitalWrite(pin_StepperX_DIR, HIGH);
			pulseOnce(pin_StepperX_PUL, interval);
		}
	}
	



}

class Point {
public:
	int x; 
	int y;
	bool IsDone;

	void SetPosition(int a,int b) {
		x = a; y = b;
	}
	void LinearMoveto(int targetX, int targetY, int dt) {
	
	}

private:


};