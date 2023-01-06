#include <Arduino.h>
#include "SSD1306Wire.h"
#include "Ticker.h"
#include <EncButton.h>



EncButton<EB_TICK, 12, 13, 14> enc;  // установка энкодера  A gpio12  / B gpio13  / SW gpio14
SSD1306Wire display(0x3c, SDA, SCL);  // ADDRESS,     SDA gpio4 / SCL gpio5   
const uint16_t acp=300;   // константа уровня adc  контроля рабочей дуги
String myStr;             //переменная  строковая для вывода на экран int
bool disp=1;              // переменная разрешение вывести на дисппей
int8_t menu=0;            //переменная  меню режима работы
int8_t old_menu=4;        //переменная  меню режима работы вспомогательная
int8_t rezim =0;          //переменная  меню выбора строки
int8_t rezim_old =4;      //переменная  меню выбора строки вспомогательная
uint8_t imp=150;          //переменная  время импульса
uint8_t imp_old=200;      //переменная  время импульса вспомогательная
uint8_t seria=1;          //переменная  время между импульсами
uint8_t seria_old=200;    //переменная  время между импульсами вспомогательная
uint16_t in=0;            //переменная  ADC ВХОДНОЕ НАПРЯЖЕНИЕ КОНТРОЛЯ ДУГИ
uint16_t zaderzka=0;      //переменная  перещитаная задержка мещду импульсами
bool i=0;                 //переменная   переманная тригера кнопки старт вспомогательная 
bool i_old=0;             //переменная   переманная тригера кнопки старт
bool start=0;             //переменная  поджог дуги
void tim();                //....   установка.....
Ticker timer1(tim, 3, 0);//  и настройка таймера


void setup() {
display.init();            // иницилизация дисплея
timer1.start();            // старт таймера
pinMode(10, OUTPUT); pinMode(D4, OUTPUT);      //  Выход на включения сварки ................................ /GPIO 10
pinMode(A0, INPUT);        //  Вход ADC датчика тока для контроля зажигания рабочей дуги  /ADC 0
pinMode(D8, INPUT_PULLUP); //  Вход с педали старт сварки................................./GPIO 9
digitalWrite(10, 1);digitalWrite(D4, 1) ;//  выключаем сварку однократно при старте для исключения самопроизвольного поджога




}

void loop() {
timer1.update();           //  перезапуск таймера






//........................................ режим работы .......................................................................


switch (menu)
{
case 0:// Режим просто сварка TIG

if (digitalRead(D8)){digitalWrite(10, 0); digitalWrite(D4, 0) ;}else{digitalWrite(10, 1); digitalWrite(D4, 1) ;}    // читаем gpio15 и включаем или выключаем дугу

break;
case 1://Режим холодной точечной сварки cold_tig

if (digitalRead(D8)){i=1 ;}else{i=0;start=0;}    //тригер кнопки старт срабатывает один раз при нажатии кнопки  
 if(i!=i_old)  {i_old=i; if (i==1)start=1;  }    //или сбрасывает её 
 if (start==1){digitalWrite(10, 0) ; digitalWrite(D4, 0) ;            // поджог дежурной дуги
if(in<acp){delay(imp);digitalWrite(10,1); digitalWrite(D4, 1) ;start=0;}// определение наличее тока рабочей дуги отсчет времени и тушение дуги сброс тригера старт
}
else{start=0;digitalWrite(10,1);digitalWrite(D4, 1) ;}                //Тушение дуги если кнопка отпущена

break;
case 2://Режим холоной точечной сварки серия импульсов cold_tig_serial

if (digitalRead(D8)){i=1 ;}else{i=0;start=0;}    //тригер кнопки старт срабатывает один раз при нажатии кнопки  
 if(i!=i_old)  {i_old=i; if (i==1)start=1;  }    //или сбрасывает её 
 if (start==1){digitalWrite(10, 0) ; digitalWrite(D4, 0) ;             // поджог дежурной дуги
if(in<acp){delay(imp);digitalWrite(10,1); digitalWrite(D4, 1) ;delay(zaderzka);start=1;}// определение наличее тока рабочей дуги отсчет времени и тушение дуги сброс тригера старт
}// добавлена серия импульсов 
else{start=0;digitalWrite(10,1); digitalWrite(D4, 1) ;}                //Тушение дуги если кнопка отпущена 			

break;
}










 //................................................  меню и обработка дисплея ..................................................
if (menu!=old_menu)  disp=1;     //  флаг обновления дисплея   смена режима работы сварки
if (rezim!=rezim_old) disp=1;    //  флаг обновления дисплея   смена меню дисплея
if (imp!=imp_old) disp=1;        //  флаг обновления дисплея   изменение длинны импульса
if (seria!=seria_old) disp=1;    //  флаг обновления дисплея   изменение длительности задержки между импульсами
if(disp==1)                      //  проверяем флаг обновления дисплея
{                                   
                       if (menu>2)menu=0;   // запираем меню 
                       if (menu<0)menu=2;   // между 0..2
                          old_menu=menu;
                       if (rezim>2)rezim=0;  // запираем режим
                       if (rezim<0)rezim=2;  // между 0..2
                          rezim_old=rezim;
                       if (imp>250)imp=250;  // запираем режим
                       if (imp<10)imp=10;  // между 10..250 ms
                          imp_old=imp;
                          if (seria>5)seria=5;  // запираем режим
                       if (seria<1)seria=1;  // между 1..5сек
                          seria_old=seria;


						            disp=0;                 // обнулить флаг обновления дисплея


                        display.clear();        //стереть дисплей
	          		 		switch (menu)
	          		 		{
	          		 		case 0:
	          		 			//режим работы 1
	          		 		   
                       display.setFont(ArialMT_Plain_16); 
                      display.drawString(20, 0, "       TIG");
                       
	          		 		  break;
	          		 		case 1:
	          		 			//режим работы 2
       		 			     
                        display.setFont(ArialMT_Plain_16);
                      display.drawString(0, 0, "     COOL_TIG");
                       
	          		 		  break;
	          		 		case 2:

	          		 			//режим работы 3
       		 			      
                       display.setFont(ArialMT_Plain_16);
                       display.drawString(0, 0, "COOL_SERIAL");
                      
       		 			       break;
	          		 		  }
                       
                       
	          		 		

 

                    
                     
	          		 		switch (rezim)
	          		 		{
	          		 		case 0:
	          		 			//строка 1
	          		 		   
                       display.setFont(ArialMT_Plain_16); 
                      display.drawString(120, 0, "<");
                       
	          		 		  break;
	          		 		case 1:
	          		 			//строка 2
       		 			     
                        display.setFont(ArialMT_Plain_16); 
                        display.drawString(120, 16, "<");
	          		 		  break;
	          		 		case 2:

	          		 			//строка 3
       		 			      
                       display.setFont(ArialMT_Plain_16); 
                       display.drawString(120,37, "<");
                      
       		 			       break;
	          		 		  }

                         display.drawString(0, 16, "PULSE           ms");
                       myStr = String(imp);
                       display.drawString(70, 16, myStr);

                       display.drawString(0, 37,   "DELAY        sek");
                       myStr = String(seria);
                       zaderzka=seria*1000;
                       display.drawString(70, 37, myStr);

							  display.display(); // Выводим  из буфера на дисплей
}



//...................................... конец обработки дисплея.................................................................
//.................................... обработка валкодера  начало...............................................................
 

  enc.tick();                        //    опрос валкодера  
if (enc.left())                      // поворот налево
   {
switch (rezim) 
{
case 0:
menu=menu+1; 	        	    	//установка режим +
break;

case 1:
imp=imp+1;          		 			    //установка длительность импульса +
break;								
case 2:
seria=seria+1;         		 			   //установка интервал между импульсами +
 break;
}     
  }



if (enc.right())                     // поворот направо
  {
switch (rezim) 
{
case 0:
menu=menu-1; 	           		 	//установка режим -
break;

case 1:
imp=imp-1;                      //установка длительность импульса - 
break;

case 2:
seria=seria-1;	          		 			   //установка интервал между импульсами -
break;
}     
  }

  if (enc.click()) rezim=rezim+1;    // меню кнопка 
 //.......................................  конец обработки валкодера ........................................................... 
  
          // myStr = String(in);
           // display.drawString(0, 10, myStr);
           // display.display(); // Выводим на 








}



void tim()  // таймер 
{
  
   
in=analogRead (A0);        //  считывание adc ток наличие рабочей дуги




}