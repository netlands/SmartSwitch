// This #include statement was automatically added by the Particle IDE.
#include "PietteTech_DHT/PietteTech_DHT.h"

// Legacy tinker support
int tinkerDigitalRead(String pin);
int tinkerDigitalWrite(String command);
int tinkerAnalogRead(String pin);
int tinkerAnalogWrite(String command);

//VARIABLES HERE
unsigned long lastTime = 0UL;

int switchon(String command);
int switchoff(String command);

#define DHTTYPE  DHT22       // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   2           // Digital pin for communications
#define RELAYPIN   6         

//declaration
void dht_wrapper(); // must be declared before the lib initialization

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);

// This wrapper is in charge of calling
// must be defined like this for the lib work
void dht_wrapper() {
    DHT.isrCallback();
}

char h1[10];  // humidity string
char t1[10];  // temperature string  

void setup()
{
	Spark.function("digitalread", tinkerDigitalRead);
	Spark.function("digitalwrite", tinkerDigitalWrite);
	Spark.function("analogread", tinkerAnalogRead);
	Spark.function("analogwrite", tinkerAnalogWrite);


	//SETUP CODE HERE
    pinMode(DHTPIN, INPUT_PULLUP);
    pinMode(RELAYPIN, OUTPUT);

    Spark.function("switchon", switchOn);
	Spark.function("switchoff", switchOff);
    Spark.function("status", Status);
    Spark.function("Switch", Switch);
    
    Spark.variable("humidity", &h1, STRING);
    Spark.variable("temperature", &t1, STRING);    
    
}

void loop()
{
	//LOOP CODE HERE
	unsigned long now = millis();
    //Publish data every 15 seconds 
    if (now-lastTime>15000UL) {
        lastTime = now;

        int result = DHT.acquireAndWait();

        switch (result) {
            case DHTLIB_OK:
                Serial.println("OK");
                break;
            case DHTLIB_ERROR_CHECKSUM:
                Serial.println("Error\n\r\tChecksum error");
                break;
            case DHTLIB_ERROR_ISR_TIMEOUT:
                Serial.println("Error\n\r\tISR time out error");
                break;
            case DHTLIB_ERROR_RESPONSE_TIMEOUT:
                Serial.println("Error\n\r\tResponse time out error");
                break;
            case DHTLIB_ERROR_DATA_TIMEOUT:
                Serial.println("Error\n\r\tData time out error");
                break;
            case DHTLIB_ERROR_ACQUIRING:
                Serial.println("Error\n\r\tAcquiring");
                break;
            case DHTLIB_ERROR_DELTA:
                Serial.println("Error\n\r\tDelta time to small");
                break;
            case DHTLIB_ERROR_NOTSTARTED:
                Serial.println("Error\n\r\tNot started");
                break;
            default:
                Serial.println("Unknown error");
                break;
        }

        sprintf(h1, "%.1f", DHT.readHumidity()); // convert Float to String
        sprintf(t1, "%.1f", DHT.readTemperature());

        // Serial.println(DHT.getHumidity(), 2);
        Spark.publish("humidity", String(h1), 60, PRIVATE);
        // Serial.println(DHT.getCelsius(), 2);
        Spark.publish("temperature", String(t1), 60, PRIVATE);
        // Publish state of switch
        // Spark.publish("switch", String(digitalRead(RELAYPIN)), 60, PRIVATE);
    }    




}



int tinkerDigitalRead(String pin) {
	int pinNumber = pin.charAt(1) - '0';
	if (pinNumber< 0 || pinNumber >7) return -1;
	if(pin.startsWith("D")) {
		pinMode(pinNumber, INPUT_PULLDOWN);
		return digitalRead(pinNumber);}
	else if (pin.startsWith("A")){
		pinMode(pinNumber+10, INPUT_PULLDOWN);
		return digitalRead(pinNumber+10);}
	return -2;}

int tinkerDigitalWrite(String command){
	bool value = 0;
	int pinNumber = command.charAt(1) - '0';
	if (pinNumber< 0 || pinNumber >7) return -1;
	if(command.substring(3,7) == "HIGH") value = 1;
	else if(command.substring(3,6) == "LOW") value = 0;
	else return -2;
	if(command.startsWith("D")){
		pinMode(pinNumber, OUTPUT);
		digitalWrite(pinNumber, value);
		return 1;}
	else if(command.startsWith("A")){
		pinMode(pinNumber+10, OUTPUT);
		digitalWrite(pinNumber+10, value);
		return 1;}
	else return -3;}

int tinkerAnalogRead(String pin){
	int pinNumber = pin.charAt(1) - '0';
	if (pinNumber< 0 || pinNumber >7) return -1;
	if(pin.startsWith("D")){
		pinMode(pinNumber, INPUT);
		return analogRead(pinNumber);}
	else if (pin.startsWith("A")){
		pinMode(pinNumber+10, INPUT);
		return analogRead(pinNumber+10);}
	return -2;}

int tinkerAnalogWrite(String command){
	int pinNumber = command.charAt(1) - '0';
	if (pinNumber< 0 || pinNumber >7) return -1;
	String value = command.substring(3);
	if(command.startsWith("D")){
		pinMode(pinNumber, OUTPUT);
		analogWrite(pinNumber, value.toInt());
		return 1;}
	else if(command.startsWith("A")){
		pinMode(pinNumber+10, OUTPUT);
		analogWrite(pinNumber+10, value.toInt());
		return 1;}
	else return -2;}
	

int switchOn(String command) {
	pinMode(RELAYPIN, OUTPUT);
	digitalWrite(RELAYPIN, 1);
	return 1;}

int switchOff(String command) {
	pinMode(RELAYPIN, OUTPUT);
	digitalWrite(RELAYPIN, 0);
	return 0;}	
	
	
int Switch(String args)
{
  int status_code = 0;  
  char id = args.charAt(0);

  if (args == "ON" || args == "on" || args == "1"){
    digitalWrite(RELAYPIN, HIGH); 
    status_code = 1;
  }

  if (args == "OFF" || args == "off" || args == "0"){
    digitalWrite(RELAYPIN, LOW);
    status_code = 0;
  }
  return status_code;
}

int Status(String args) {
    int status_code = 2;
    if (digitalRead(RELAYPIN) == 0) {
        status_code = 0;
    }
    if (digitalRead(RELAYPIN) == 1) {
        status_code = 1;
    }   
    return status_code;
}

