#include <Wire.h>

const int MPU = 0x68;
#define CALLIBRATION_VALUES 1000

int16_t acc_x =0, acc_y=0, acc_z=0; // accelerometer raw data
float acc_x_real, acc_y_real, acc_z_real; // converted accelerations in G's
int16_t gyro_x, gyro_y, gyro_z; // gyro raw data 
float gyro_x_real, gyro_y_real, gyro_z_real; // converted gyro values in deg/s

// error values 
float x_acc_sum =0, y_acc_sum=0, z_acc_sum=0;
float x_gyro_sum=0, y_gyro_sum=0, z_gyro_sum=0;
float x_acc_err=0, y_acc_err=0, z_acc_err=0;
float x_gyro_err=0, y_gyro_err=0, z_gyro_err=0;

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

// calculate accelerometer error for callibration
void callibrateIMU() {
  // make the mean of 200 values
  for(int a=0; a<CALLIBRATION_VALUES; a++) {
    // read the accelerometer
    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    
    Wire.requestFrom(0x68, 6, true);
    // todo: change divider factor based on the FS reading set
    acc_x = (float)(Wire.read()<<8|Wire.read());
    acc_y = (float)(Wire.read()<<8|Wire.read());
    acc_z = (float)(Wire.read()<<8|Wire.read());

    // accelerometer running sum 
    x_acc_sum += acc_x;
    y_acc_sum += acc_y;
    z_acc_sum += acc_z;

//    // read the gyroscope
//    Wire.beginTransmission(0x68);
//    Wire.write(0x43);
//    Wire.endTransmission(false); 
//
//    Wire.requestFrom(0x68, 6, true);
//    // todo: change divisor based on the FS reading set
//    gyro_x = (Wire.read()<<8|Wire.read())/32.8;
//    gyro_y = (Wire.read()<<8|Wire.read())/32.8;
//    gyro_z = (Wire.read()<<8|Wire.read())/32.8;
//
//    // gyroscope running sum
//    x_gyro_sum += gyro_x;
//    y_gyro_sum += gyro_y;
//    z_gyro_sum += gyro_z;    
  }

  // DEBUG
 printf("Sum: %.2f, %.2f, %.2f \n",
      x_acc_sum,
      y_acc_sum,
      z_acc_sum);
  
  // find the average error in all the 3 axes
  // for accelerometer 
  x_acc_err = x_acc_sum / CALLIBRATION_VALUES;
  y_acc_err = y_acc_sum / CALLIBRATION_VALUES;
  z_acc_err = z_acc_sum / CALLIBRATION_VALUES;

  printf("Error values: %.2f, %.2f, %.2f \n",
      x_acc_err,
      y_acc_err,
      z_acc_err);

  // for the gyroscope
//  x_gyro_err = x_gyro_sum / CALLIBRATION_VALUES;
//  y_gyro_err = y_gyro_sum / CALLIBRATION_VALUES;
//  z_gyro_err = z_gyro_sum / CALLIBRATION_VALUES;  
}

void setup() {
  Serial.begin(115200);
  mpu_init();

  // callibrate
  Serial.println("Callibrating the IMU...");
  callibrateIMU();
  Serial.println("Callibration done!");
  
//  printf("%.2f, %.2f, %.2f \n",
//      x_acc_err,
//      y_acc_err,
//      z_acc_err);
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

//  printf("ACC: %.2f,%.2f, %.2f GY:%.2f,%.2f,%.2f\n",
//          acc_x_real - x_acc_err, 
//          acc_y_real - y_acc_err,
//          acc_z_real - z_acc_err,
//          gyro_x_real,
//          gyro_y_real,
//          gyro_z_real
//          );
           
  delay(50);
  
}
