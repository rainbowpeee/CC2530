#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I			0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取

 
//****************************************
//函数声明
//****************************************
void WriteSDA_0(void);
void WriteSDA_1(void);
void WriteSCL_0(void);
void WriteSCL_1(void);
void ReadSDA(void);
void SDA_OUT(void);
void Init_IO(void);
void MPU6050_Start(void);
void MPU6050_Stop(void);
void MPU6050_SendACK(unsigned char ack);
unsigned char MPU6050_RecvACK(void);
void MPU6050_SendByte(unsigned char data);
unsigned char MPU6050_RecvByte(void);
void Single_WriteI2C(unsigned char REG_Address,unsigned char REG_data);
unsigned char Single_ReadI2C(unsigned char REG_Address);
void InitMPU6050(void);
void ALLInit(void);
void Multiple_read_MPU6050(void);
void delay_us(unsigned int u);
int GetData(unsigned char REG_Address);
unsigned int absi(int val);