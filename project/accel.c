#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>

#include "accel.h"

static int steps = 0;
static bool sampleSteps = true;
static pthread_t accelThreadId;
static bool sex = 0;
// get height in cm
static float weight = 0;
static int height = 0;
static int distancePerStep = 0;
static int distanceWalked = 0;
static float caloriesBurnt;

// const
float VO2 = 11.546;
float minPerStepsRate = 9.8 / 1000;

static int updateSteps(double magnitude);
static void updateDistancePerStep();
static void calculateDistanceWalked();
static void calculateCaloriesBurnt();

// int main(){
//   int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_ACCEL_ADDRESS);
//     while(1) {
//       readRawData(i2cFileDesc);
// 	}
// }

// *************** ACCELEROMETER DEVICE SETUP ******************//

static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));

    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
    // Now read the value and return it
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));
    if (res != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr,unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
    perror("I2C: Unable to write i2c register.");
    exit(1);
}
}

static int initI2cBus(char* bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);

    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

double getMagnitude(double x, double y, double z){

        static double power2 = 2;

        double xSquared = pow((double)x,power2);
        double ySquared = pow((double)y,power2);
        double zSquared = pow((double)z,power2);

         double sum = xSquared + ySquared + zSquared;
         double magnitude = sqrt(sum);
        return magnitude; 
}

double formatRawData( int data0, int data1){

    static double maxResolution = 256.00;
    data1 = data1 << 8;
    int dataOut = data0 + data1;

    double data = (double) dataOut / maxResolution;
    return data;
}


void readRawData(int i2cFileDesc ){
    int x0 = readI2cReg(i2cFileDesc,REG_DATAX0);
    int x1 = readI2cReg(i2cFileDesc,REG_DATAX1);

    int y0 = readI2cReg(i2cFileDesc,REG_DATAY0);
    int y1 = readI2cReg(i2cFileDesc,REG_DATAY1);

    int z0 = readI2cReg(i2cFileDesc,REG_DATAZ0);
    int z1 = readI2cReg(i2cFileDesc,REG_DATAZ1);

        
    int xOut = formatRawData(x0,x1);
    int yOut = formatRawData(x0,x1);
    double zOut = formatRawData(z0,z1);

    double mag = getMagnitude(xOut,yOut,zOut);
    int steps = updateSteps(mag);

    
    // printf("Total steps: %d \n", steps);
    sleep(1);
}

// *************** ACCELEROMETER STATS ******************//


static int updateSteps(double magnitude){
    double threshold = 310;
    if (magnitude > threshold){
        steps++;
    }
    // calculateDistanceWalked();
    return steps;
}   

static void updateDistancePerStep() {
    // sex == 0 is male
    if(sex == 0) {
        distancePerStep = height * 0.413;
    }
    // sex == 1 is female
    else {
        distancePerStep = height * 0.415;
    }
}

static void calculateDistanceWalked() {
    updateDistancePerStep();
    distanceWalked = steps * distancePerStep;
}

static void calculateCaloriesBurnt() {
    // assume walking speed of 3mph on a zero inclide
    float calPerMin = ((VO2 * weight) / 100)*5;
    // want to calculate minute spent walking, so steps * min / step
    float minutesWalking = steps * minPerStepsRate;
    caloriesBurnt = minutesWalking * calPerMin;
}

// *************** ACCELEROMETER STATS GETTERS & SETTERS ******************//

int getSteps() {
    printf("MAG: %d", steps);
    return steps;
}

int getCalories() {
    calculateCaloriesBurnt();
    printf("calories %d burnt \n", caloriesBurnt);
    return caloriesBurnt;
}

int getDistanceInKm() {
    // return as km
    calculateDistanceWalked();
    printf("distance %d\n walked", distanceWalked);
    return distanceWalked;
}

void setHeight(int heightToSet) {
    printf("height has been set from accel !!!!");
    height = heightToSet;
}

void setWeight(int weightToSet) {
    printf("weight has been set from accel!!!!");
    weight = weightToSet;
}

// *************** THREAD INITIALIZERS ******************//

static void *accelThread(void *__) {
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_ACCEL_ADDRESS);
    while(sampleSteps) {
      readRawData(i2cFileDesc);
	}   
}


void accel_init() {
    sampleSteps = true;
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_ACCEL_ADDRESS);
    writeI2cReg(i2cFileDesc, 0x2d,0x08);
    pthread_create(&accelThreadId, NULL, accelThread, NULL);
}

void accel_cleanup() {
    sampleSteps = false;
    pthread_join(accelThreadId, NULL);
}


