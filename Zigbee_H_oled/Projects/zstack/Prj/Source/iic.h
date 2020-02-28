


 
//****************************************
//º¯ÊýÉùÃ÷
//****************************************
void delay_us(unsigned int u);
void WriteSDA_0(void);
void WriteSDA_1(void);
void WriteSCL_0(void);
void WriteSCL_1(void);
void ReadSDA(void);
void SDA_OUT(void);
void Init_IO(void);
void OLED_Start(void);
void OLED_Stop(void);
void OLED_SendACK(unsigned char ack);
unsigned char OLED_RecvACK(void);
void OLED_SendByte(unsigned char data);
unsigned char OLED_RecvByte(void);
void Single_WriteI2C(unsigned char REG_Address,unsigned char REG_data);
unsigned char Single_ReadI2C(unsigned char REG_Address);