#define ADC_BASE 0xFF204000  
#define SW_BASE 0xFF200040
#define Mask_12_bits 0x00000FFF 
#define KEY_BASE 0xFF200050


volatile int* const pushBtnInput = (int*)KEY_BASE;
volatile int * ADC_BASE_ptr = (int *)ADC_BASE;
int mask = Mask_12_bits;
int season;


struct RGB {
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

//Suppose we have a date value, we need to convert this date to a season
//This table will contain various seasons
const unsigned short seasonalTable[4][12] =
  {{5,7,9,10,11,13,15,16,17,18,19,20},
  {16,17,18,19,20,21,22,23,24,25,26,30},
  {7,8,9,10,13,14,15,16,17,18,19,20},
  {-25,-20,-15,-13,-10,-8,-4,-1,0,1,3,5},
  }; //try to do 


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

//convert an ADC value to a temperature, return this value
int getTemperature() {
    unsigned char temp1=0, temp2=0, temp3=0, deg=0, i=0, near1=0, near2=0;
	int ADC = getADC();
	int celsius = 20;
	//Loop through temperature array, look for the specific ADC value
	for (int i = 1; i < 101; i++)
	{
		//If current index is the ADC value we return the index as the index proves to be the celsius temperature 
		if ((temperaturetable[i]>=ADC) && (ADC>=temperaturetable[i+1]) ) {
			near1 = temperaturetable[i]-ADC;
			near2 = ADC-temperaturetable[i+1];
			if(near1<near2) celsius=i;
			else  celsius=i+1;
		}
	}

	return celsius;
}

int getADC() {
	*(ADC_BASE_ptr) =0; // write anything to channel 0 to update ADC
//	*(ADC_BASE_ptr+1) =0;
	volatile int channel0;
	channel0 = (*(ADC_BASE_ptr) & mask);
	//channel2 = (*(ADC_BASE_ptr +1) & mask);

	//Return ADC value of channel 0
	return channel0;	
}

//returns a season based on push button input
int getSeason(){
    volatile int inputValue = *pushBtnInput;
        
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
	
	return 1;
    

}




//Return an RGB value appropriate to a specific seasonal input(0->3), with temperature input
int getRGB(int temp,int season) {
	printf("data");
	
    struct RGB *rgb = malloc(12*sizeof(struct RGB));
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
    
	unsigned char i=0, near1=0, near2=0;
	int index = 0;
	//Loop through through the relevant seasonal RGB array
	for (int i = 1; i < 12; i++)
	{
		//Find closest temperature value index
		if ((seasonalTable[season][i]>=temp) && (temp>=seasonalTable[season][i+1]) ) {
			near1 = seasonalTable[season][i]-temp;
			near2 = temp-seasonalTable[season][i+1];
			if(near1<near2) index=i;
			else  index=i+1;
		}
	} 

	//return the RGB value at the specific index
	//it will indicate specific ambient lighting value for a season
	printf("the R G B of the black color is %d %d %d\n",rgb[index].R,rgb[index].G,rgb[index].B);
	free(rgb);
}


//power function
int power(int x, int y){
      if(y == 0)
        return 1;
     return (x * power(x,y-1) );
    }

int main(void)
{
	season = getSeason();
	
	
	

	while(1) {
		

		//printf("input was %d",season);

		//R
		getRGB(getTemperature(),season);
		
	}
	
	//Setup time interval to loop for, check 
}		

