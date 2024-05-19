#include <Wire.h>

const int MPU = 0x68;

int16_t acc_x, acc_y, acc_z; // accelerometer raw data
float acc_x_real, acc_y_real, acc_z_real; // converted accelerations in G's
int16_t gyro_x, gyro_y, gyro_z; // gyro raw data 
float gyro_x_real, gyro_y_real, gyro_z_real; // converted gyro values in deg/s

// error values 
float acc_error = 0;

void mpu_init() {
  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  delay(50);

  Wire.beginTransmission(0x68);
  Wire.write(0x1B);   // send to gyro address
  Wire.write(0x10);  // 1000 deg/s full scale
  Wire.endTransmission(true);

  Wire.beginTransmission(0x68);
  Wire.write(0x1C); // config accel_register
  Wire.write(0x00); // set to +-2g full scale
  Wire.endTransmission(true);
}

// calculate accelerometer error
float calculate_acc_error() {
  // make the mean of 200 values
  if(acc_error=0) {
    for(int a=0; a<200; a++) {
      Wire.beginTransmission(0x68);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(0x68, 6, true);
      acc_x = (Wire.read()<<8|Wire.read())/4096.0;
      acc_y = (Wire.read()<<8|Wire.read())/4096.0;
      acc_z = (Wire.read()<<8|Wire.read())/4096.0;
      
    }
  }
}

void setup() {
  Serial.begin(115200);
  mpu_init();
 
}

void loop() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);// I2C restart

  Wire.requestFrom(MPU, 6, true);
  acc_x = Wire.read()<<8 | Wire.read();
  acc_y = Wire.read()<<8 | Wire.read();
  acc_z = Wire.read()<<8 | Wire.read();
  acc_x_real = (float) acc_x / 16384.0;
  acc_y_real = (float) acc_y / 16384.0; 
  acc_z_real = (float) acc_z / 16384.0;

  // read gyroscope
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU, 6, true);
  gyro_x = Wire.read()<<8 | Wire.read();
  gyro_y = Wire.read()<<8 | Wire.read();
  gyro_z = Wire.read()<<8 | Wire.read();
  gyro_x_real = (float) gyro_x / 32.8;
  gyro_y_real = (float) gyro_y / 32.8;
  gyro_z_real = (float) gyro_z / 32.8;

  printf("ACC: %.2f,%.2f GY:%.2f,%.2f,%.2f\n",
          acc_x_real, 
          acc_y_real,
          gyro_x_real,
          gyro_y_real,
          gyro_z_real
          );
           
  delay(50);
  
}
