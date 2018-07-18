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
int posX=0, posY=0, tgtX[100], tgtY[100], kX, kY;
int StepperX_multiplier = 1;
int StepperY_multiplier = 1;
int StepperX_pulsePerRotation = 400; //��̨�����ת������;
int StepperY_pulsePerRotation = 400; //��������ת������;
int Globalinterval = 1000; //ȫ��ʱ���� ΢��


class Point {
public:

	bool IsDone = true;
	bool IsMoving = false;

	void SetCurrentPositionAs(int a, int b) {
		x = a; y = b;
	}

	void ReportPosition() {
		Serial.print(x); Serial.print(","); Serial.println(y);

	}

	void ReturnZero() {
		Serial.println("Finding Zero point of X axis, please stand by...");
		digitalWrite(pin_StepperX_DIR, LOW); //��̨����
		delay(100);
		while (!digitalRead(pin_Switch1_switch)) { //����δ������ʱ
			Pulse(pin_StepperX_PUL, 500);
		}

		delay(1000);
		digitalWrite(pin_StepperX_DIR, HIGH);
		for (int i = 0; i < StepperX_pulsePerRotation; i++) {
			Pulse(pin_StepperX_PUL, 500);
		}
		Serial.println("Zero point of X axis found.");
		SetCurrentPositionAs(0, 0);
		Serial.println("Set Current Position to 0,0");
	}

	void driveConstSpeed(int pin_PUL, bool dir, float speed)   //��dir��������pin_PUL
	{
		IsDone = false;
		int periodmicros = 1/speed;

		if (pin_PUL = pin_StepperX_PUL)
		{
			digitalWrite(pin_StepperX_DIR, dir);                     //��̨�͵�ƽ��0��low������
			Pulse(pin_PUL, periodmicros);
			if (!dir)
			{														 //dir=0��̨����
				x = x - 1;
			}
			else
			{
				x = x + 1;
			}
		}

		if (pin_PUL = pin_StepperY_PUL)
		{
			digitalWrite(pin_StepperY_DIR, dir);                     //����͵�ƽ��0��low�����
			Pulse(pin_PUL, periodmicros);
			if (!dir)
			{														//dir=0���������ɴ
				y = y - 1;
			}
			else
			{
				y = y + 1;
			}
		}
		ReportPosition();
		IsDone = true;
	}

	void Drive(int pin_PUL, bool dir, int periodmicros)   //��dir��������pin_PUL
	{
		IsMoving = true;
		if (pin_PUL = pin_StepperX_PUL)
		{
			digitalWrite(pin_StepperX_DIR, dir);                     //��̨�͵�ƽ��0��low������
			Pulse(pin_PUL, periodmicros);
			if (!dir)
			{														 //dir=0��̨����
				x = x - 1;
			}
			else
			{
				x = x + 1;
			}
		}

		if (pin_PUL = pin_StepperY_PUL)
		{
			digitalWrite(pin_StepperY_DIR, dir);                     //����͵�ƽ��0��low�����
			Pulse(pin_PUL, periodmicros);
			if (!dir)
			{														//dir=0���������ɴ
				y = y - 1;
			}
			else
			{
				y = y + 1;
			}
		}
		ReportPosition();
		IsMoving = false;
	}

	void LinearMoveto(int targetX, int targetY, int dt) { //dtΪ����ʱ�䣬��λΪ΢��
		IsDone = false;
		bool kx; //X���˶�������
		bool ky; //y���˶�������
		int F;	//ֱ�߲岹ƫ��
		int sigma = targetX + targetY - x - y; //��ƫ����-��������
		float distance = sqrt((targetX - x) ^ 2 + (targetX - y) ^ 2);
		bool dir;
		int localInterval = dt / sigma;

		if (targetX >= x & targetY >= y) //��һ����ֱ��
		{
			kx = 1;
			ky = 1;
		}
		if (targetX < x & targetY >= y) //�ڶ�����
		{
			kx = 0;
			ky = 1;
		}
		if (targetX < x & targetY < y) //��������
		{
			kx = 0;
			ky = 0;
		}
		if (targetX >= x & targetY < y) //��������
		{
			kx = 0;
			ky = 1;
		}
		while (sigma != 0)
		{

			if (F >= 0)
			{
				Drive(X, kx, localInterval);
				F = F - targetY;
			}
			else
			{
				Drive(Y, ky, localInterval);
				F = F + targetX;
			}
			sigma = targetX + targetY - x - y;
		}
		IsDone = true;
	}


	void LinearMovetoCSpeed(int targetX, int targetY, float speed) { //speed�ĵ�λΪ����/΢�� ӦΪ����/1000/1000
		IsDone = false;
		
		bool kx; //X���˶�������
		bool ky; //y���˶�������
		int F;	//ֱ�߲岹ƫ��
		int sigma = targetX + targetY - x - y; //��ƫ����-��������
		int dt = sigma / speed;
		float distance = sqrt((targetX - x) ^ 2 + (targetX - y) ^ 2);
		bool dir;
		int localInterval = dt / sigma;

		if (targetX >= x & targetY >= y) //��һ����ֱ��
		{
			kx = 1;
			ky = 1;
		}
		if (targetX < x & targetY >= y) //�ڶ�����
		{
			kx = 0;
			ky = 1;
		}
		if (targetX < x & targetY < y) //��������
		{
			kx = 0;
			ky = 0;
		}
		if (targetX >= x & targetY < y) //��������
		{
			kx = 0;
			ky = 1;
		}
		while (sigma != 0)
		{

			if (F >= 0)
			{
				Drive(X, kx, localInterval);
				F = F - targetY;
			}
			else
			{
				Drive(Y, ky, localInterval);
				F = F + targetX;
			}
			sigma = targetX + targetY - x - y;
		}
		IsDone = true;
	}
private:
	int x;
	int y;

};



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
	Point M;
	M.ReturnZero();
	while (1) {
		//M.LinearMoveto(1000, 1000, 50000000);
		delay(1000);
		//M.LinearMoveto(0, 0, 50000000);
		delay(1000);
	}

	//manualControl();
}




void Pulse(int pin, int intevalus)  //(pin,periodmicros)���͵��ʷ������� ��������intevalus����λ΢��
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
	Serial.print(posX);	Serial.print(","); Serial.println(posY);
	
	if (offset>20) 
	{
		int interval = 3000;
		if (offsetX_Joystick1 > 20) {
			digitalWrite(pin_StepperX_DIR, LOW);
			Pulse(pin_StepperX_PUL, interval);
			posX = posX - 1;

		}
		if (offsetX_Joystick1 < -20) {
			digitalWrite(pin_StepperX_DIR, HIGH);
			Pulse(pin_StepperX_PUL, interval);
			posX = posX + 1;
		}
		if (offsetY_Joystick1 > 20) {
			digitalWrite(pin_StepperY_DIR, LOW);
			Pulse(pin_StepperY_PUL, interval);
			posY = posY - 1;

		}
		if (offsetY_Joystick1 < -20) {
			digitalWrite(pin_StepperY_DIR, HIGH);
			Pulse(pin_StepperY_PUL, interval);
			posY = posY + 1;
		}
	}
}
