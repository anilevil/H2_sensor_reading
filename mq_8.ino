#define MQ8 14
#define RL_VALUE 10    //define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR 9.21  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                                     //which is derived from the chart in datasheet
 
/***********************Software Related Macros************************************/
#define CALIBARAION_SAMPLE_TIMES 50   //define how many samples you are going to take in the calibration phase
#define CALIBRATION_SAMPLE_INTERVAL 500   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define READ_SAMPLE_INTERVAL 50    //define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES 5     //define the time interal(in milisecond) between each samples in 
                                                     //normal operation
#define GAS_H2 0
int a1
float H2Curve[3] = {2.3, 0.93,-1.44};    //two points are taken from the curve in datasheet. 
                                                     //with these two points, a line is formed which is "approximately equivalent" 
                                                     //to the original curve. 
                                                     //data format:{ x, y, slope}; point1: (lg200, lg8.5), point2: (lg10000, lg0.03) 
 
float Ro = 10;                  //Ro is initialized to 10 kilo ohms
 
void setup()
{
  Serial.begin(9600);                                //UART setup, baudrate = 9600bps
  Serial.begin(115200);
  Serial.print("Calibrating...\n");                
  Ro = MQCalibration(MQ8);                        //Calibrating the sensor. Please make sure the sensor is in clean air 
                                                     //when you perform the calibration                    
  Serial.print("Calibration is done...\n"); 
}
 
void loop()
{
   Serial.print("H2:"); 
   Serial.print(MQGetGasPercentage(MQRead(MQ8)/Ro,GAS_H2) );
   Serial.print( "ppm" );
   Serial.print("\n");
   delay(200);
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}
 
float MQCalibration(int mq8)
{
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculation(analogRead(mq8));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   //calculate the average value
 
  val = val/RO_CLEAN_AIR_FACTOR;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                        //according to the chart in the datasheet 
 
  return val; 
}

float MQRead(int mq8)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq8));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}
 
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id == GAS_H2) {
     return MQGetPercentage(rs_ro_ratio,H2Curve);
  }  
  return 0;
}
 
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
