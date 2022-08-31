#include <Arduino.h>

void AE_HX711_Init(char ch);
void AE_HX711_Reset(char ch);
long AE_HX711_Read(char ch);
long AE_HX711_Averaging(char num, char ch);
float AE_HX711_getGram(char num, char ch);
float tiltCalc(float *y1, float *y2);

//ここに5kgを乗せたときの真値を入力する(左から順に1~6)
float M5[6] = {5.6f, 4.4f, 5.6f, 5.6f, 5.7f, 5.5f};

//ここに20kgを乗せたときの真値を入力する(左から順に1~6)
float M20[6] = {22.4f, 18.1f, 22.6f, 22.8f, 22.9f, 22.4f};

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
  tiltCalc(M5, M20);
  Serial.begin(9600);
  Serial.println("AE_HX711 test");
  for (ch = 0; ch < MAX_CH; ch++)
  {
    AE_HX711_Init(ch);
    AE_HX711_Reset(ch);
    sprintf(s, "calibrating %d", ch);
    offset[ch] = AE_HX711_getGram(30, ch);
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
float tiltCalc(float *y1, float *y2)
{
  float numerator = 0;
  for (int i = 0; i < 6; i++)
  {
    numerator = 0;
    numerator += 5 * y1[i] + 20 * y2[i];
    correctionValue[i] = numerator / 425;
  }
}
