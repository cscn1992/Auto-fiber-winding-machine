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




int pin_Joystick1_X = A0; //ҡ�˿�����X����
int pin_Joystick1_Y = A1; //ҡ�˿�����Y����
int pin_StepperX_PUL = 8;  //StepperX_��̨���_PUL_��������
int pin_StepperX_DIR = 9;	//StepperX_��̨���_PUL_��������
int pin_StepperY_PUL = 10;  //StepperY�������ߵ��_PUL_��������
int pin_StepperY_DIR = 11;	//StepperY�������ߵ��_PUL_��������
int pin_Switch1_switch = 12;  //��λ������
int pin_Switch2_switch = 13;  //��λ������
int X = pin_StepperX_PUL;	//X�ᣨ��̨�������������
int Y = pin_StepperY_PUL;	//X�ᣨ���ߵ������������
int posX, posY, tgtX[100], tgtY[100], kX, kY;
int StepperX_multiplier = 1;
int StepperY_multiplier = 1;
int StepperX_pulsePerRotation = 400; //��̨�����ת������;
int StepperY_pulsePerRotation = 400; //��������ת������;
int Globalinterval = 1000; //ȫ��ʱ���� ΢��


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

int linearto(int tgtx,int tgty, int dtime) {	//���Բ岹�ƶ���tgtx,tgty,��ʱdtime��΢�룩 
	bool kx; //X���˶�������
	bool ky; //y���˶�������
	int F;	//ֱ�߲岹ƫ��
	int sigma = tgtx + tgty - posX - posY; //��ƫ����-��������
	float distance = sqrt((tgtx-posX)^2+(tgty-posY)^2);
	bool dir;
	int localInterval = dtime / sigma;

	if (tgtx >= posX & tgty >= posY) //��һ����ֱ��
	{
		kx = 1;
		ky = 1;
	}
	if (tgtx < posX & tgty >= posY) //�ڶ�����
	{
		kx = 0;
		ky = 1;
	}
	if (tgtx < posX & tgty < posY) //��������
	{
		kx = 0;
		ky = 0;
	}
	if (tgtx >= posX & tgty < posY) //��������
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

void drive(int pin_PUL, bool dir, int periodmicros)   //��dir��������pin_PUL
{
	if (pin_PUL = pin_StepperX_PUL)
	{
		digitalWrite(pin_StepperX_DIR, dir);                     //��̨�͵�ƽ��0��low������
		pulseOnce(pin_PUL, periodmicros);
		if (!dir)
		{														 //dir=0��̨����
			posX = posX - 1;
		}
		else
		{
			posX = posX + 1;
		}
	}

	if (pin_PUL = pin_StepperY_PUL)
	{
		digitalWrite(pin_StepperY_DIR, dir);                     //����͵�ƽ��0��low�����
		pulseOnce(pin_PUL, periodmicros);
		if (!dir)
		{														//dir=0���������ɴ
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
	digitalWrite(pin_StepperX_DIR, LOW); //��̨����
	delay(100);
	while (!digitalRead(pin_Switch1_switch)) { //����δ������ʱ
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

void pulseOnce(int pin, int intevalus)  //(pin,periodmicros)���͵��ʷ������� ������periodmicros����λ΢��
{
	digitalWrite(pin, HIGH);
	delayMicroseconds(intevalus / 2);
	digitalWrite(pin, LOW);
	delayMicroseconds(intevalus / 2);
}

int sgn(int x)
{
	if (x == 0) //��x==0ʱ����������0
		return 0;
	else if (x > 0) //��x>0ʱ����������1
		return 1;
	else  //��x<0ʱ���������أ�1
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