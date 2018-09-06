
#include <xc.h>
#include <pic16f18313.h>


//コンフィグレーションビッツ
// CONFIG1
#pragma config FEXTOSC = OFF    
#pragma config RSTOSC = HFINT32 
#pragma config CLKOUTEN = OFF   
#pragma config CSWEN = OFF      
#pragma config FCMEN = OFF      

// CONFIG2
#pragma config MCLRE = ON      
#pragma config PWRTE = OFF      
#pragma config WDTE = OFF       
#pragma config LPBOREN = OFF    
#pragma config BOREN = OFF      
#pragma config BORV = LOW       
#pragma config PPS1WAY = OFF    
#pragma config STVREN = OFF     
#pragma config DEBUG = OFF      

// CONFIG3
#pragma config WRT = OFF        
#pragma config LVP = OFF        

// CONFIG4
#pragma config CP = OFF         
#pragma config CPD = OFF        


#define _XTAL_FREQ 32000000 //動作周波数は32MHz  
#define SLOW 5              //減衰値の定数(減衰遅め)
#define MIDDLE 7           //減衰値の定数(中くらい)
#define FAST 10             //減衰値の定数(早め)


unsigned char volume = 0;
unsigned char damping = 0;      //50msごとの減衰値


void so4(char length);void ra4(char length);void si4(char length);
void do5(char length);void re5(char length);void mi5(char length);void fa5(char length);
void so5(char length);void ra5(char length);void si5(char length);
void do6(char length);void re6(char length);void mi6(char length);
void rest(char length);

void ongakuka(void);
void menuet(void);
void kouka(void);



void __interrupt() isr(void){
    TMR1H = 0x3c;
    TMR1L = 0xb0;                       //1us * (65536-15536) = 50ms
    PIR1bits.TMR1IF = 0;              // Timer1フラグクリア
    volume = volume-damping;                
    if(volume == 251 || volume == 252 || volume == 0){
        PIE1bits.TMR1IE = 0;           //volume設定値255-(減衰値(damping)10*26) = -5  unsigned charの256-5 = 251を認識し,それ以降割り込みを止める
                                       //同じようにdamping = 8のときも同様にすれば0になる
        volume = 0;                    
    }
}

void main(void){
    ANSELA  = 0;
    TRISA = 0;
    PORTA = 0;
    
    //PWM設定
    RA5PPS = 0b00001100;
    CCP1CON = 0b10011111;   //CCP1をPWMモードに設定（かつCCP1の下位2ビットを0に設定）,データ左詰め
    T2CON = 0x00;
    PR2 = 255;              
    CCPR1L = CCPR1H = 0;    
    T2CONbits.TMR2ON = 1;
    
    //割り込み設定
    //Timer1設定。50msを作る
    T1CON = 0x21;  
    TMR1H = 0x3c;
    TMR1L = 0xb0;                       //これで1us * (65536-15536) = 50ms
    PIR1bits.TMR1IF = 0;              // Timer1フラグクリア
    PIE1bits.TMR1IE = 0;     //Timer1割り込みは各音階が始まる際に有効にする。
    
    PEIE = 1;
    GIE = 1;
    
    __EEPROM_DATA(0x00,0,0,0,0,0,0,1);		//EEPROMを使いボタンが押されたときの曲を決める
    unsigned char ee_select;
    
    while(1){
        ee_select = eeprom_read(0x00);
        if(ee_select == 0x01){
            __delay_ms(20);  //チャタリングやプログラムミスで短時間のうちにEEPROMが書き換えられるのを防ぐ
            eeprom_write(0x00,0x02);
            menuet();
            SLEEP();		//終了後はスリープ
        }
        if(ee_select == 0x02){
            __delay_ms(20);
            eeprom_write(0x00,0x03);
            ongakuka();
            SLEEP();
        }
        else if(ee_select == 0x03){
            __delay_ms(20);
            eeprom_write(0x00,0x04);//曲やイルミネーションの途中でボタンを押したらelseへ
            kouka();
            eeprom_write(0x00,0x01);//最後まで動作させたら次ボタンが押された時はmenuetへ
            SLEEP();
        }
        else{
            __delay_ms(20);
            eeprom_write(0x00,0x01);
            mi5(16);do5(16);
            LATA = 0;
            SLEEP();
        }
    } 
}

//音階(ドレミ)の関数
//引数(char length)は16分音符なら「16」、4分音符なら「4」のように使用

void so4(char length){
	int j;
	volume = 255;
	LATA = 0b110;

	if(length == 1){
		j = 784;
		damping = SLOW;
	}
	else if(length == 2){
		j = 392;
		damping = SLOW;
	}
	else if(length == 3){
		j = 294;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 196;
		damping = FAST;
	}
	else if(length == 6){
		j = 147;
		damping = FAST;
	}
	else if(length == 8){
		j = 98;
		damping = FAST;
	}
	else if(length == 16){
		j = 49;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(1270);
		CCPR1H = 0;
		__delay_us(1270);
	}
	LATA = 0;
}
void ra4(char length){
	int j;
	volume = 255;
	LATA = 0b101;

	if(length == 1){
		j = 880;
		damping = SLOW;
	}
	else if(length == 2){
		j = 440;
		damping = SLOW;
	}
	else if(length == 3){
		j = 330;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 220;
		damping = FAST;
	}
	else if(length == 6){
		j = 165;
		damping = FAST;
	}
	else if(length == 8){
		j = 110;
		damping = FAST;
	}
	else if(length == 16){
		j = 55;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(1132);
		CCPR1H = 0;
		__delay_us(1132);
	}
	LATA = 0;
}
void si4(char length){
	int j;
	volume = 255;
	LATA = 0b111;

	if(length == 1){
		j = 976;
		damping = SLOW;
	}
	else if(length == 2){
		j = 488;
		damping = SLOW;
	}
	else if(length == 3){
		j = 366;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 244;
		damping = FAST;
	}
	else if(length == 6){
		j = 183;
		damping = FAST;
	}
	else if(length == 8){
		j = 122;
		damping = FAST;
	}
	else if(length == 16){
		j = 61;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(1012);
		CCPR1H = 0;
		__delay_us(1012);
	}
	LATA = 0;
}
void do5(char length){
	int j;
	volume = 255;
	LATA = 0b001;

	if(length == 1){
		j = 1040;
		damping = SLOW;
	}
	else if(length == 2){
		j = 520;
		damping = SLOW;
	}
	else if(length == 3){
		j = 390;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 260;
		damping = FAST;
	}
	else if(length == 6){
		j = 195;
		damping = FAST;
	}
	else if(length == 8){
		j = 130;
		damping = FAST;
	}
	else if(length == 16){
		j = 65;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(950);
		CCPR1H = 0;
		__delay_us(950);
	}
	LATA = 0;
}
void re5(char length){
	int j;
	volume = 255;
	LATA = 0b010;

	if(length == 1){
		j = 1168;
		damping = SLOW;
	}
	else if(length == 2){
		j = 584;
		damping = SLOW;
	}
	else if(length == 3){
		j = 438;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 292;
		damping = FAST;
	}
	else if(length == 6){
		j = 219;
		damping = FAST;
	}
	else if(length == 8){
		j = 146;
		damping = FAST;
	}
	else if(length == 16){
		j = 73;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(851);
		CCPR1H = 0;
		__delay_us(851);
	}
	LATA = 0;
}
void mi5(char length){
	int j;
	volume = 255;
	LATA = 0b100;

	if(length == 1){
		j = 1312;
		damping = SLOW;
	}
	else if(length == 2){
		j = 656;
		damping = SLOW;
	}
	else if(length == 3){
		j = 492;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 328;
		damping = FAST;
	}
	else if(length == 6){
		j = 246;
		damping = FAST;
	}
	else if(length == 8){
		j = 164;
		damping = FAST;
	}
	else if(length == 16){
		j = 82;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(758);
		CCPR1H = 0;
		__delay_us(758);
	}
	LATA = 0;
}
void fa5(char length){
	int j;
	volume = 255;
	LATA = 0b011;

	if(length == 1){
		j = 1392;
		damping = SLOW;
	}
	else if(length == 2){
		j = 696;
		damping = SLOW;
	}
	else if(length == 3){
		j = 522;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 348;
		damping = FAST;
	}
	else if(length == 6){
		j = 261;
		damping = FAST;
	}
	else if(length == 8){
		j = 174;
		damping = FAST;
	}
	else if(length == 16){
		j = 87;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(715);
		CCPR1H = 0;
		__delay_us(715);
	}
	LATA = 0;
}
void so5(char length){
	int j;
	volume = 255;
	LATA = 0b110;

	if(length == 1){
		j = 1568;
		damping = SLOW;
	}
	else if(length == 2){
		j = 784;
		damping = SLOW;
	}
	else if(length == 3){
		j = 588;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 392;
		damping = FAST;
	}
	else if(length == 6){
		j = 294;
		damping = FAST;
	}
	else if(length == 8){
		j = 196;
		damping = FAST;
	}
	else if(length == 16){
		j = 98;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(633);
		CCPR1H = 0;
		__delay_us(633);
	}
	LATA = 0;
}
void ra5(char length){
	int j;
	volume = 255;
	LATA = 0b101;

	if(length == 1){
		j = 1760;
		damping = SLOW;
	}
	else if(length == 2){
		j = 880;
		damping = SLOW;
	}
	else if(length == 3){
		j = 660;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 440;
		damping = FAST;
	}
	else if(length == 6){
		j = 330;
		damping = FAST;
	}
	else if(length == 8){
		j = 220;
		damping = FAST;
	}
	else if(length == 16){
		j = 110;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(568);
		CCPR1H = 0;
		__delay_us(568);
	}
	LATA = 0;
}
void si5(char length){
	int j;
	volume = 255;
	LATA = 0b111;

	if(length == 1){
		j = 1968;
		damping = SLOW;
	}
	else if(length == 2){
		j = 984;
		damping = SLOW;
	}
	else if(length == 3){
		j = 738;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 492;
		damping = FAST;
	}
	else if(length == 6){
		j = 369;
		damping = FAST;
	}
	else if(length == 8){
		j = 246;
		damping = FAST;
	}
	else if(length == 16){
		j = 123;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(506);
		CCPR1H = 0;
		__delay_us(506);
	}
	LATA = 0;
}
void do6(char length){
	int j;
	volume = 255;
	LATA = 0b001;

	if(length == 1){
		j = 2080;
		damping = SLOW;
	}
	else if(length == 2){
		j = 1040;
		damping = SLOW;
	}
	else if(length == 3){
		j = 780;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 520;
		damping = FAST;
	}
	else if(length == 6){
		j = 390;
		damping = FAST;
	}
	else if(length == 8){
		j = 260;
		damping = FAST;
	}
	else if(length == 16){
		j = 130;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(473);
		CCPR1H = 0;
		__delay_us(473);
	}
	LATA = 0;
}
void re6(char length){
	int j;
	volume = 255;
	LATA = 0b010;

	if(length == 1){
		j = 2336;
		damping = SLOW;
	}
	else if(length == 2){
		j = 1168;
		damping = SLOW;
	}
	else if(length == 3){
		j = 876;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 584;
		damping = FAST;
	}
	else if(length == 6){
		j = 438;
		damping = FAST;
	}
	else if(length == 8){
		j = 292;
		damping = FAST;
	}
	else if(length == 16){
		j = 146;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(422);
		CCPR1H = 0;
		__delay_us(422);
	}
	LATA = 0;
}
void mi6(char length){
	int j;
	volume = 255;
	LATA = 0b100;

	if(length == 1){
		j = 2624;
		damping = SLOW;
	}
	else if(length == 2){
		j = 1312;
		damping = SLOW;
	}
	else if(length == 3){
		j = 984;
		damping = MIDDLE;
	}
	else if(length == 4){
		j = 656;
		damping = FAST;
	}
	else if(length == 6){
		j = 492;
		damping = FAST;
	}
	else if(length == 8){
		j = 328;
		damping = FAST;
	}
	else if(length == 16){
		j = 164;
		damping = FAST;
	}
	else
		return;

	PIE1bits.TMR1IE = 1;

	for(int i=0;i<j;i++){
		CCPR1H = volume;
		__delay_us(379);
		CCPR1H = 0;
		__delay_us(379);
	}
	LATA = 0;
}

//休符
void rest(char length){
    if(length == 16)
        __delay_ms(125);
    else if(length == 8)
        __delay_ms(250);
    else if(length == 6)
        __delay_ms(375);
    else if(length == 4)
        __delay_ms(500);
    else if(length == 3)
        __delay_ms(750);
    else if(length == 2)
        __delay_ms(1000);
    else if(length == 1)
        __delay_ms(2000);
    else 
        return;
}

//以下、楽譜データ。
//著作権の関係上、「山の音楽家」と「東京電機大学　校歌」は関係ないメロディにさせていただきます。ご了承ください。

void ongakuka(void){
	do5(4);mi5(4);so5(4);do6(1);
    
}
void menuet(void){
    so5(4);do5(8);re5(8);mi5(8);fa5(8);
    so5(4);do5(4);do5(4);
    ra5(4);fa5(8);so5(8);ra5(8);si5(8);
    do6(4);do5(4);do5(4);
    fa5(4);so5(8);fa5(8);mi5(8);re5(8);
    mi5(4);fa5(8);mi5(8);re5(8);do5(8);
    si4(4);do5(8);re5(8);mi5(8);do5(8);
    re5(8);so5(8);fa5(8);mi5(8);re5(4);
    
    so5(4);do5(8);re5(8);mi5(8);fa5(8);
    so5(4);do5(4);do5(4);
    ra5(4);fa5(8);so5(8);ra5(8);si5(8);
    do6(4);do5(4);do5(4);
    fa5(4);so5(8);fa5(8);mi5(8);re5(8);
    mi5(4);fa5(8);mi5(8);re5(8);do5(8);
    re5(4);mi5(8);re5(8);do5(8);si4(8);
    do5(2);rest(4);
        
}
void kouka(void){
	do5(4);mi5(4);so5(4);do6(1);
    
}
