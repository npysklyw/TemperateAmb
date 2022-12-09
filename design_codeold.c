#define ADC_BASE 0xFF204000  
#define SW_BASE 0xFF200040
#define Mask_12_bits 0x00000FFF 
#define KEY_BASE 0xFF200050
#define MPCORE_PRIV_TIMER   0xFFFEC600 



volatile int* const pushBtnInput = (int*)KEY_BASE;
volatile int* const MODE_SWITCH = (int*)SW_BASE;
volatile int * ADC_BASE_ptr = (int *)ADC_BASE;
const int countInterval = 200000000;
volatile unsigned int currentMillis = 0;
volatile unsigned int lapTime = 0;
#include <stdio.h>
#include <math.h>


//Some variables
int mask = Mask_12_bits;
int season;

typedef struct timer{
    int loadVal;
    int count;
    int control;
    int status;
}timer;

volatile timer* const MILLISECOND_TIMER = (timer*)MPCORE_PRIV_TIMER ;
struct RGB {
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

void checkInput();
void setTimer();
void lapTimer();
void clearTimer();
void startTimer();
void stopTimer();
void diplayNumber();


//Table that maps low to high temps of seasons Spring -> Winter
//0 - Spring, 3 - Winter, 
//Note: temperature isfor Toronto
const unsigned short seasonalTable[4][12] =
  {{5,7,9,10,11,13,15,16,17,18,19,20},
  {16,17,18,19,20,21,22,23,24,25,26,30},
  {7,8,9,10,13,14,15,16,17,18,19,20},
  {-25,-20,-15,-13,-10,-8,-4,-1,0,1,3,5},
  }; 

//read at intervals

//Define a set of ADC values that map to index
//Index of the value is the degree celsius of the environment
const unsigned short temperaturetable[101] =
  {783,774,764,754,744,734,724,713,702,692,
   681,670,659,648,637,626,614,603,591,580,
   569,557,546,535,523,512,501,490,479,468,
   457,447,436,426,415,405,395,385,376,366,
   356,347,338,329,320,312,303,295,287,279,
   271,264,257,250,243,236,229,223,217,210,
   204,199,193,188,182,177,172,167,162,158,
   153,149,144,140,136,132,129,125,121,118,
114,111,108,105,102,99,96,94,91,89, 86,84,81,79,77,75,73,71,69,67,
65}; //10x10table

//Get the temperature based on the current ADC value
int getTemperature() {

    unsigned char near1=0, near2=0;

    //Get interger value of ADC
    //Note getADC I used in the lab, so I will not put it here(it is in the full source code however)
	int ADC = getADC();

    //Default temperature of 20 celsius
	int celsius = 20;

	//Loop through temperature array, look for the specific ADC value
	for (int i = 1; i < 101; i++)
	{
		//If current index is the ADC value we return the index as the index is the celsius temperature 
		if ((temperaturetable[i]>=ADC) && (ADC>=temperaturetable[i+1]) ) {

            //Check if the ADC value in bounds
			near1 = temperaturetable[i]-ADC;
			near2 = ADC-temperaturetable[i+1];

			if(near1<near2) celsius=i;
			else  celsius=i+1;
		}
	}

    //Return value in celsius
	return celsius;
}

long decimalToBinary(int decimalnum)
{
    long binarynum = 0;
    int rem, temp = 1;

    while (decimalnum!=0)
    {
        rem = decimalnum%2;
        decimalnum = decimalnum / 2;
        binarynum = binarynum + rem*temp;
        temp = temp * 10;
    }
    return binarynum;
}

//Get ADC 
int getADC() {

    //Write to update
	*(ADC_BASE_ptr) =0; 

    //Read from channel 0
	volatile int channel0;

    //Apply bit mask
	channel0 = (*(ADC_BASE_ptr) & mask);

	//Return ADC value of channel 0
	return channel0;	
}

//Return a season based on push button input
int getSeason(){

    //Get the current value of the push buttons
    volatile int inputValue = *pushBtnInput;
        
    //Return appropriate season for the push buttons 1-4
    switch(inputValue){
        case 1:
            return 0;
            break;
        case 2:
            return 1;
            break;
        case 4:
            return 2;
            break;
        case 8:
            return 3;
            break;
    }
	
    //If not a selected choose the season Summer
	return 1;
    
}

//Return an RGB value appropriate to a specific seasonal input(0->3) in combination with a temperature input
int getRGB(int temp,int season) {
	
    //Define a struture to hold RGB values
    struct RGB *rgb = malloc(12*sizeof(struct RGB));

    //The structure is 12 size
    //There are 12 temperature settings avaliable
    //0 is white and is coldest temperature for that season
    //12 is red, and is the hottest temperature for the season
    //I personally picked each colour code
	rgb[0].R = 255;
    rgb[0].G = 230;
    rgb[0].B = 204;

    rgb[1].R = 255;
    rgb[1].G = 217;
    rgb[1].B = 179;
    
    rgb[2].R = 255;
    rgb[2].G = 204;
    rgb[2].B = 153;
    
    rgb[3].R = 255;
    rgb[3].G = 191;
    rgb[3].B = 128;
    
    rgb[4].R = 255;
    rgb[4].G = 179;
    rgb[4].B = 102;
    
    rgb[5].R = 255;
    rgb[5].G = 166;
    rgb[5].B = 77;
    
    rgb[6].R = 255;
    rgb[6].G = 153;
    rgb[6].B = 51;
    
    rgb[7].R = 255;
    rgb[7].G = 140;
    rgb[7].B = 26;
    
    rgb[8].R = 255;
    rgb[8].G = 128;
    rgb[8].B = 0;
    
    rgb[9].R = 230;
    rgb[9].G = 115;
    rgb[9].B = 0;
    
    rgb[10].R = 204;
    rgb[10].G = 102;
    rgb[10].B = 0;
    
    rgb[11].R = 204;
    rgb[11].G = 0;
    rgb[11].B = 51;
    
    rgb[12].R = 204;
    rgb[12].G = 0;
    rgb[12].B = 0;
    
    //Similar operation to getTemperature, compare if the current temperature is in bounds of two adjacent
	unsigned char near1=0, near2=0;
	int index = 0;

	//Loop through through the relevant seasonal RGB array
    //The seasonal temperature table defined above, the relevant row is seleceted using the season function input
	for (int i = 1; i < 12; i++)
	{
		//Find closest temperature value index
		if ((seasonalTable[season][i]>=temp) && (temp>=seasonalTable[season][i+1]) ) {

            //Once again, chose a indice that is closest to current temprerature
			near1 = seasonalTable[season][i]-temp;
			near2 = temp-seasonalTable[season][i+1];
			if(near1<near2) index=i;
			else  index=i+1;
		}
	} 

    //Index now has indice that will be the best suited colour to the seasonal temperature.

	//Return the RGB value at this index as #,#,#
    return rgb[index].R,rgb[index].G,rgb[index].B;
	
}


void setTimer(int interval){
    MILLISECOND_TIMER->loadVal= interval;
};

void lapTimer(){
    lapTime = currentMillis;
}

void clearTimer(){
    currentMillis = 0;
}

void startTimer(){
    MILLISECOND_TIMER->control = 3;
}

void stopTimer(){
    MILLISECOND_TIMER ->control = 0;
}

//A function that checks the currentMillis to see if the timer is at intervals of 10 seconds
void checkInterval() {
	 
     //Use modulo to see if remainder 10
	 if (currentMillis % 10 == 0) {
		 
                //Debug line used to check current time
				printf("Current time surpassed is %d seconds\n",currentMillis);

                //Get the current RGB colur value based on temp, season
				getRGB(getTemperature(),season);
				
		}
        
        //Increment the tiemr if the timer timeouts
        if (MILLISECOND_TIMER->status == 1){
			
            currentMillis++;

            //Reset
            MILLISECOND_TIMER ->status = 1;
        }
        
}


//This method is very dependant on what peripheral is used
//The hope is to use the rgb values to write a specific sequence of lights to be on
int setColour(r,g,b) {

	//We would conduct writing of generated RGB values to the specific bulb peripheral here
    //ATM we have a skelton method though to show where this would happen
	 printf("Red Value: %ld \n", decimalToBinary(r));
	 printf("Green Value: %ld \n", decimalToBinary(g));
	 printf("Blue Value: %ld \n", decimalToBinary(g));

}


int main(void)
{
     //Get the current season
	season = getSeason();
	
    //Setup a timer and start it
	setTimer(countInterval);
	startTimer();

	while(1) {	
		
        //Check interval for 10 seconds
		checkInterval();
	
	}
	
}		

