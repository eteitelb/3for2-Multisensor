//code from TEKANAN multisensor. Additional sensor code with kit-specific 
//calibrations are available on the particle IDE

#include "math.h"

//SHT75 Temperature RH Setup
#include "SHT1x.h"
#define dataPin  D0
#define clockPin D1
SHT1x sht1x(dataPin, clockPin);
double temp = 0;
double hum = 0;

//CO2 Setup
int co2_input = A0;
double CO2 = 0;

//PM2.5 Setup
double PART = 0;
int part_input = 5;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 3000;//sampe 30s ;
unsigned long lowpulseoccupancy = 0;
float PART_ratio = 0;
float concentration = 0;

//MiCS-5524 CO Setup
int co_input = A4;
double CO = 0;
double CO_Vo = 4.219934869;
double CO_scale = 0.6474878444;

//MiCS-2714 NO2 Setup
int no2_input = A2; //NO2 connection pin
double NO2 = 0;
double NO2_Vo = 2.777744211;
double NO2_scale = 0.649020004;

//MiCS-2614 O3 Setup
int o3_input = A3;
double O3 = 0;
double O3_Vo = 4.239825236;
double O3_scale =0.6466946275;

//TGS2602 VOC Setup
int voc_input = A1;
double VOC = 0;
double VOC_Vo = 0.03069270888;
double VOC_scale = 0.6466346154;

//Grove Formaldehyde Setup
double FORM = 0;
int form_input = A7;
double FORM_Vo = 0.0399191931;
double FORM_scale = 0.6492834395;

//general setup
double v_m = 3.30;
double v_c = 4.99;
int dataset = 1;

void setup() {
    pinMode(no2_input,INPUT);
    pinMode(co2_input,INPUT);
    pinMode(co_input,INPUT);
    pinMode(voc_input,INPUT);
    pinMode(form_input,INPUT);
    pinMode(o3_input,INPUT);
    pinMode(part_input,INPUT);
    pinMode(A5,INPUT);
    Spark.variable("temp", &temp, DOUBLE);
    Spark.variable("hum", &hum, DOUBLE);
    Spark.variable("NO2", &NO2, DOUBLE);
    Spark.variable("CO2", &CO2, DOUBLE);
    Spark.variable("CO", &CO, DOUBLE);
    Spark.variable("VOC", &VOC, DOUBLE);
    Spark.variable("FORM", &FORM, DOUBLE);
    Spark.variable("O3", &O3, DOUBLE);
    Spark.variable("PART", &PART, DOUBLE);
    starttime = millis();//get the current time;
}

double NO2_read(){
    double NO2_voltage = 0;
    for(int i=0; i<dataset; i++){ //for loop to average analog reading
        NO2_voltage += double((analogRead(no2_input)*v_m)/4095/NO2_scale);
        delay(5);
    }
    NO2_voltage = NO2_voltage/dataset;
    double ratio = ((v_c/NO2_voltage)-1)/((v_c/NO2_Vo)-1);//compare to baseline reading
    double ppm = 0.14990374*ratio+0.002799;//convert to NO2 ppm
    return ppm;
}

double CO_read(){
    double CO_voltage = 0;
    for(int i=0; i<dataset; i++){ //for loop to average analog reading
        CO_voltage += double((analogRead(co_input)*v_m)/4095/CO_scale);
        delay(5);
    }
    CO_voltage = CO_voltage/dataset;
    double ratio = ((v_c/CO_voltage)-1)/((v_c/CO_Vo)-1);//compare to baseline reading
    double ppm = 4.1011*pow(ratio,-1.155);//convert to CO ppm
    return ppm;
}

double O3_read(){
    double O3_voltage = 0;
    for(int i=0; i<dataset; i++){ //for loop to average analog reading
        O3_voltage += double((analogRead(o3_input)*v_m)/4095/O3_scale);
        delay(5);
    }
    O3_voltage = O3_voltage/dataset;
    double ratio = ((v_c/O3_voltage)-1)/((v_c/O3_Vo)-1);//compare to baseline reading
    double ppm = 12.04232153 * pow(ratio,2) + 66.40948162*ratio + 14.29624716;//convert to O3 ppb
    return ppm;
}

double VOC_read(){
    double VOC_voltage = 0;
    for(int i=0; i<dataset; i++){ //for loop to average analog reading
        VOC_voltage += double((analogRead(voc_input)*v_m)/4095/VOC_scale);
        delay(5);
    }
    VOC_voltage = VOC_voltage/dataset;
    double ratio = ((v_c/VOC_voltage)-1)/((v_c/VOC_Vo)-1);//compare to baseline reading
    double ppm = 0.1407*(1/pow(ratio,1.676));//convert to O3 ppb
    return ppm;
}

double FORM_read(){
    double FORM_voltage = 0;
    for(int i=0; i<dataset; i++){ //for loop to average analog reading
        FORM_voltage += double((analogRead(form_input)*v_m)/4095/FORM_scale);
        delay(5);
    }
    FORM_voltage = FORM_voltage/dataset;
    double ratio = ((v_c/FORM_voltage)-1)/((v_c/FORM_Vo)-1);//compare to baseline reading
    double ppm = 1.2742*(1/pow(ratio,2.315));//convert to O3 ppb
    return ppm;
}

double CO2_read(){
    double CO2_voltage = 0;
    for(int i=0; i<dataset; i++){ //for loop to average analog reading
        CO2_voltage += double((analogRead(co2_input)*v_m)/4095);
        delay(5);
    }
    CO2_voltage = CO2_voltage/dataset;
    double ppm = -263069.8047 * pow(CO2_voltage,3) + 2081835.488 * pow(CO2_voltage,2) - 5489455.175 * CO2_voltage + 4823599.224;//convert to CO2 ppb
    //double ppm = 582380000000000 * pow(CO2_voltage, -29.55291642);
    return ppm;
}

unsigned long pulseIn(uint16_t pin, uint8_t state) {
    
    GPIO_TypeDef* portMask = (PIN_MAP[pin].gpio_peripheral); // Cache the target's peripheral mask to speed up the loops.
    uint16_t pinMask = (PIN_MAP[pin].gpio_pin); // Cache the target's GPIO pin mask to speed up the loops.
    unsigned long pulseCount = 0; // Initialize the pulseCount variable now to save time.
    unsigned long loopCount = 0; // Initialize the loopCount variable now to save time.
    unsigned long loopMax = 20000000; // Roughly just under 10 seconds timeout to maintain the Spark Cloud connection.
    
    // Wait for the pin to enter target state while keeping track of the timeout.
    while (GPIO_ReadInputDataBit(portMask, pinMask) != state) {
        if (loopCount++ == loopMax) {
            return 0;
        }
    }
    
    // Iterate the pulseCount variable each time through the loop to measure the pulse length; we also still keep track of the timeout.
    while (GPIO_ReadInputDataBit(portMask, pinMask) == state) {
        if (loopCount++ == loopMax) {
            return 0;
        }
        pulseCount++;
    }
    
    // Return the pulse time in microseconds by multiplying the pulseCount variable with the time it takes to run once through the loop.
    return pulseCount * 0.405; // Calculated the pulseCount++ loop to be about 0.405uS in length.
}

double PART_read(){
    duration = pulseIn(part_input, LOW);
    lowpulseoccupancy = lowpulseoccupancy+duration;

    if ((millis()-starttime) > sampletime_ms)//if the sample time == 30s
    {
        PART_ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
        concentration = 1.1*pow(PART_ratio,3)-3.8*pow(PART_ratio,2)+520*PART_ratio+0.62; // using datasheet curve
        lowpulseoccupancy = 0;
        starttime = millis();
    }
    double ppm = double(concentration);
    return ppm;
}

void loop() {
    float temp_c = sht1x.readTemperatureC();
    float humidity = sht1x.readHumidity();
    
    temp=double(temp_c);
    hum = double(humidity);
    PART = PART_read();
    NO2 = NO2_read(); //NO2 concentration in ppm
    CO = CO_read(); //CO concentration in ppm
    O3 = O3_read(); //O3 concentration in ppb
    VOC = VOC_read(); //toluene concentration in ppm
    FORM = FORM_read(); //formaldehyde concentration in ppm
    CO2 = CO2_read();
    

    delay(500);
}