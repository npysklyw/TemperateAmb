#define I2C0_BASE		0xFFC04000

//Functions to define communciations
void init_I2C( void );
unsigned char read_over_I2C( unsigned char address );
void write_over_I2C( unsigned char address, unsigned char value );
void read_over_I2C_multiple( unsigned char address, unsigned char* values, unsigned char length );
void init_GS( void );

//Structure to communicate with I2C
typedef struct _I2Cn
{
	// uses variables called 'pad' to align
	// needed registers in memory
		int control;			//0x00
		int target; 			//0x04
		int slave;				//0x08
		int pad0; 				//skip
		int data_cmd;			//0x10
		int std_scl_hcnt;		//0x14
		int std_scl_lcnt;		//0x18
		int fast_scl_hcnt;		//0x1C
		int fast_scl_lcnt;		//0x20
		int pad1;				//skip
		int pad2;				//skip
		int intr_status;		//0x2C
		int intr_mask;			//0x30		
		int raw_intr_status;	//0x34
		int rx_fifo_thr;		//0x38
		int tx_fifo_thr;		//0x3C
		int cmb_intr;			//0x40
		int rx_under_intr;		//0x44
		int rx_over_intr;		//0x48
		int tx_over_intr;		//0x4C
		int intr_read;			//0x50
		int tx_abort_intr;		//0x54
		int rx_done_intr;		//0x58
		int activity_intr;		//0x5C
		int stop_dtct_intr;		//0x60
		int start_dtct_intr;	//0x64
		int gen_call_intr;		//0x68
		int enable;				//0x6C
		int status;				//0x70
		int tx_fifo_lvl;		//0x74
		int rx_fifo_lvl;		//0x78
		int sda_hold;			//0x7C
		int tx_abort_src;		//0x80
		int gen_slave_nack;		//0x84
		int dma_control;		//0x88
		int dma_tx_lvl;			//0x8C
		int rx_data_lvl;		//0x90
		int sda_setup;			//0x94
		int ack_gen_call;		//0x98
		int enable_status;		//0x9C
		int ss_fs_supp;			//0xA0
} I2Cn;

// make I2C pointer global for easy access
volatile I2Cn* const I2C_ptr = ( I2Cn* )I2C0_BASE;






//Initialize the I2C of the DE10 board
void init_I2C()
{
	// set I2C enable to abort any transmissions
	// and disable device
	//1 step 1
	//Set the enable to pin
	I2C_ptr->enable = 2;
	// wait until this is done
	// enable status register goes to 0 when finished
	
	//CHECK THE enable status register until it is set to 0
	while ( I2C_ptr->enable_status & 0x1 )
		;
	

	//Write apprioraaite control bits to the the control register
	//ensure that device has 7bit addressing mode 
	I2C_ptr->control = 0b01101101;

	// set target address
	//Set the slave address, address for slave device is 0x44
	I2C_ptr->target = 0x53;

	//Not 100% sure about this
	// set count for high-period of clock
	I2C_ptr->fast_scl_hcnt = 90;
	// set count for low-period of clock
	I2C_ptr->fast_scl_lcnt = 160;
	// uses fast clock because control register set to [xxxx x10x]
	// set to [xxxx x01x] for slow clock
	// note these high- and low-periods are determined by
	// ADXL345:	high-period has minimum 60	 (x10 ns)
	//          low-period has minimum 130 (x10 ns)
	// here we padded both to total 250 = 2.5 us, i.e. the period of 
	// SCL fast clock (400 KHz)
	// other devices may have different requirements for high-
	// and low-periods, so check documentation

	//Enable the IC20 program
	// turn on IC20
	I2C_ptr->enable = 1;
	// wait until this is done, or unitl enabler status set to 1
	while (( I2C_ptr->enable_status & 0x1 ) == 0 )
		;	
}


//Found in  device manual
//I follow section 6.3.11
void initAS621() {

	//Writing start byte to address 0 as said in manual to start temperautre reading
	I2Cwrite(0,1);

	//Send somethign to configuration address
	I2Cwrite(0,1);


}

void tempRead() {

	//First set the index pointer to the TVAL register
	I2Cwrite(0,1);

	//Two read requests made
	//ONe is the 15 -> 8 for the msb of 16 bit temp
	int msb, lsb;
	//Read from tempval register
	msb = I2Cread();
	
	//Next is for the lsb
	//Bits 7-> 0
	//read the temperature

	//We actually have a tmeperature table givwn, which I will put as an array
	//Goes from tempeartures 100 -> -40,
}

void I2Cwrite( unsigned char address, unsigned char value ) {

	//Write to device address
	I2C_ptr->data_cmd = address + 0x400;

	//Write value to register
	I2C_ptr->data_cmd = value;

}

void I2Cread() {

	I2C_ptr->data_cmd = address + 0x400;

	//Switch to read mode

	//Wait until data read

	return I2C_ptr->data_cmd;
}

