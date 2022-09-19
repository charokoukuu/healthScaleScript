#include <Arduino.h>

void AE_HX711_Init(char ch);
void AE_HX711_Reset(char ch);
long AE_HX711_Read(char ch);
long AE_HX711_Averaging(char num, char ch);
float AE_HX711_getGram(char num, char ch);
float tiltCalc(float *M2, float *M5, float *M7, float *M10, float *M12, float *M15, float *M17, float *M20, float *M22, float *M25, float *M27, float *M30, float *M32, float *M35, float *M37);

//以下に真値を貼り付けてください
float M2[6] = {2.263f, 1.807f, 2.26f, 2.273f, 2.317f, 2.253f};
float M5[6] = {5.59f, 4.513f, 5.637f, 5.687f, 5.743f, 5.613f};
float M7[6] = {7.863f, 6.32f, 7.893f, 7.963f, 8.043f, 7.843f};
float M10[6] = {11.227f, 9.003f, 11.263f, 11.353f, 11.473f, 11.197f};
float M12[6] = {13.463f, 10.813f, 13.52f, 13.637f, 13.77f, 13.437f};
float M15[6] = {16.833f, 13.517f, 16.9f, 17.047f, 17.21f, 16.797f};
float M17[6] = {19.08f, 15.32f, 19.17f, 19.323f, 19.493f, 19.043f};
float M20[6] = {22.38f, 18.023f, 22.537f, 22.737f, 22.883f, 22.413f};
float M22[6] = {24.613f, 19.833f, 24.767f, 25.027f, 25.173f, 24.653f};
float M25[6] = {27.98f, 22.533f, 28.11f, 28.443f, 28.607f, 28.007f};
float M27[6] = {30.227f, 24.333f, 30.387f, 30.72f, 30.913f, 30.25f};
float M30[6] = {33.603f, 27.043f, 33.8f, 34.123f, 34.36f, 33.61f};
float M32[6] = {35.857f, 28.853f, 36.103f, 36.397f, 36.64f, 35.853f};
float M35[6] = {39.227f, 31.56f, 39.38f, 39.81f, 40.08f, 39.213f};
float M37[6] = {41.473f, 33.37f, 41.623f, 42.087f, 42.373f, 41.457f};

//---------------------------------------------------//
// ピンの設定
//---------------------------------------------------//
#define pin_dout0 2
#define pin_slk0 3
#define pin_dout1 4
#define pin_slk1 5
#define pin_dout2 6
#define pin_slk2 7
#define pin_dout3 8
#define pin_slk3 9
#define pin_dout4 10
#define pin_slk4 11
#define pin_dout5 12
#define pin_slk5 13
#define MAX_CH 6

//---------------------------------------------------//
// ロードセル　4ポイント　（薄型）　SC902　50kg [P-13043]
//---------------------------------------------------//
#define OUT_VOL 0.00053f //定格出力 [V]
#define LOAD 50000.0f    //定格容量 [g]

float offset[6];
float correctionValue[6];
void setup()
{
  char ch;
  char s[20];
  tiltCalc(M2, M5, M7, M10, M12, M15, M17, M20, M22, M25, M27, M30, M32, M35, M37);
  Serial.begin(9600);
  Serial.println("AE_HX711 test");
  for (ch = 0; ch < MAX_CH; ch++)
  {
    AE_HX711_Init(ch);
    AE_HX711_Reset(ch);
    sprintf(s, "calibrating %d", ch);
    offset[ch] = AE_HX711_getGram(10, ch);
  }
}

void loop()
{
  float data[MAX_CH];
  char S1[40 * MAX_CH];
  char sa[40], s[40];
  char ch;
  S1[0] = (char)NULL;
  strcat(S1, "RUN,");
  for (ch = 0; ch < MAX_CH; ch++)
  {
    data[ch] = AE_HX711_getGram(1, ch); // getGram()が１だと早くなる
    sprintf(sa, "%s,", dtostrf((data[ch] - offset[ch]) / correctionValue[ch], 8, 4, s));
    strcat(S1, sa);
  }
  strcat(S1, "END");
  Serial.println(S1);
  delay(100);
}

void AE_HX711_Init(char ch)
{
  switch (ch)
  {
  case 5:
    pinMode(pin_slk5, OUTPUT);
    pinMode(pin_dout5, INPUT);
    break;
  case 4:
    pinMode(pin_slk4, OUTPUT);
    pinMode(pin_dout4, INPUT);
  case 3:
    pinMode(pin_slk3, OUTPUT);
    pinMode(pin_dout3, INPUT);
  case 2:
    pinMode(pin_slk2, OUTPUT);
    pinMode(pin_dout2, INPUT);
  case 1:
    pinMode(pin_slk1, OUTPUT);
    pinMode(pin_dout1, INPUT);
  default:
    pinMode(pin_slk0, OUTPUT);
    pinMode(pin_dout0, INPUT);
  }
}

void AE_HX711_Reset(char ch)
{
  switch (ch)
  {
  case 5:
    digitalWrite(pin_slk5, 1);
    delayMicroseconds(100);
    digitalWrite(pin_slk5, 0);
    delayMicroseconds(100);
    break;
  case 4:
    digitalWrite(pin_slk4, 1);
    delayMicroseconds(100);
    digitalWrite(pin_slk4, 0);
    delayMicroseconds(100);
    break;
  case 3:
    digitalWrite(pin_slk3, 1);
    delayMicroseconds(100);
    digitalWrite(pin_slk3, 0);
    delayMicroseconds(100);
    break;
  case 2:
    digitalWrite(pin_slk2, 1);
    delayMicroseconds(100);
    digitalWrite(pin_slk2, 0);
    delayMicroseconds(100);
    break;
  case 1:
    digitalWrite(pin_slk1, 1);
    delayMicroseconds(100);
    digitalWrite(pin_slk1, 0);
    delayMicroseconds(100);
    break;
  default:
    digitalWrite(pin_slk0, 1);
    delayMicroseconds(100);
    digitalWrite(pin_slk0, 0);
    delayMicroseconds(100);
  }
}

long AE_HX711_Read(char ch)
{
  long data = 0;
  int pin_dout;
  int pin_slk;
  switch (ch)
  {
  case 5:
    pin_dout = pin_dout5;
    pin_slk = pin_slk5;
    break;
  case 4:
    pin_dout = pin_dout4;
    pin_slk = pin_slk4;
    break;
  case 3:
    pin_dout = pin_dout3;
    pin_slk = pin_slk3;
    break;
  case 2:
    pin_dout = pin_dout2;
    pin_slk = pin_slk2;
    break;
  case 1:
    pin_dout = pin_dout1;
    pin_slk = pin_slk1;
    break;
  default:
    pin_dout = pin_dout0;
    pin_slk = pin_slk0;
  }
  while (digitalRead(pin_dout) != 0)
    ;
  delayMicroseconds(10);
  for (int i = 0; i < 24; i++)

  {
    digitalWrite(pin_slk, 1);
    delayMicroseconds(5);
    digitalWrite(pin_slk, 0);
    delayMicroseconds(5);
    data = (data << 1) | (digitalRead(pin_dout));
  }
  // Serial.println(data,HEX);
  digitalWrite(pin_slk, 1);
  delayMicroseconds(10);
  digitalWrite(pin_slk, 0);
  delayMicroseconds(10);
  return data ^ 0x800000;
}

long AE_HX711_Averaging(char num, char ch)
{
  long sum = 0;
  for (int i = 0; i < num; i++)
    sum += AE_HX711_Read(ch);
  return sum / num;
}

float AE_HX711_getGram(char num, char ch)
{
#define HX711_R1 20000.0f
#define HX711_R2 8200.0f
#define HX711_VBG 1.25f
#define HX711_AVDD 4.2987f                  // AVDD=VBG*((R1+R2)/R2
#define HX711_ADC1bit HX711_AVDD / 16777216 // 16777216=(2^24)
#define HX711_PGA 128                       // Programmable Gain Amplifier
#define HX711_SCALE (OUT_VOL * HX711_AVDD / LOAD * HX711_PGA)

  float data;

  data = AE_HX711_Averaging(num, ch) * HX711_ADC1bit;
  // Serial.println( HX711_AVDD);
  // Serial.println( HX711_ADC1bit);
  // Serial.println( HX711_SCALE);
  // Serial.println( data);
  data = data / HX711_SCALE;

  return data;
}

//原点を通る回帰直線の傾き算出
float tiltCalc(float *M2, float *M5, float *M7, float *M10, float *M12, float *M15, float *M17, float *M20, float *M22, float *M25, float *M27, float *M30, float *M32, float *M35, float *M37)
{
  float numerator = 0;
  for (int i = 0; i < 6; i++)
  {
    numerator = 0;
    numerator += 2 * M2[i] + 5 * M5[i] + 7 * M7[i] + 10 * M10[i] + 12 * M12[i] + 15 * M15[i] + 17 * M17[i] + 20 * M20[i] + 22 * M22[i] + 25 * M25[i] + 27 * M27[i] + 30 * M30[i] + 32 * M32[i] + 35 * M35[i] + 37 * M37[i];
    float denominator = pow(2, 2) + pow(5, 2) + pow(7, 2) + pow(10, 2) + pow(12, 2) + pow(15, 2) + pow(17, 2) + pow(20, 2) + pow(22, 2) + pow(25, 2) + pow(27, 2) + pow(30, 2) + pow(32, 2) + pow(35, 2) + pow(37, 2);
    correctionValue[i] = numerator / denominator;
  }
}
