#include "LSM6DSOXSensor.h"
#include "lsm6dsox_activity_recognition_for_mobile.h"

#ifdef ARDUINO_SAM_DUE
#define DEV_I2C Wire1
#elif defined(ARDUINO_ARCH_STM32)
#define DEV_I2C Wire
#elif defined(ARDUINO_ARCH_AVR)
#define DEV_I2C Wire
#else
#define DEV_I2C Wire
#endif
#define SerialPort Serial

#define INT_1 A5
#define INT_1 INT_IMU

//Interrupts.
volatile int mems_event = 0;

// Components
LSM6DSOXSensor AccGyr(&DEV_I2C, LSM6DSOX_I2C_ADD_L);

// MLC
ucf_line_t *ProgramPointer;
int32_t LineCounter;
int32_t TotalNumberOfLine;

void INT1Event_cb();
void printMLCStatus(uint8_t status);

void setup() {
  uint8_t mlc_out[8];
  // Led.
  pinMode(LED_BUILTIN, OUTPUT);

  // Force INT1 of LSM6DSOX low in order to enable I2C
  pinMode(INT_1, OUTPUT);

  digitalWrite(INT_1, LOW);

  delay(200);

  // Initialize serial for output.
  SerialPort.begin(115200);
  
  // Initialize I2C bus.
  DEV_I2C.begin();

  AccGyr.begin();
  AccGyr.Enable_X();
  AccGyr.Enable_G();

  /* Feed the program to Machine Learning Core */
  /* Activity Recognition Default program */  
  ProgramPointer = (ucf_line_t *)lsm6dsox_activity_recognition_for_mobile;
  TotalNumberOfLine = sizeof(lsm6dsox_activity_recognition_for_mobile) / sizeof(ucf_line_t);
  SerialPort.println("Activity Recognition for LSM6DSOX MLC");
  SerialPort.print("UCF Number Line=");
  SerialPort.println(TotalNumberOfLine);

  for (LineCounter=0; LineCounter<TotalNumberOfLine; LineCounter++) {
    if(AccGyr.Write_Reg(ProgramPointer[LineCounter].address, ProgramPointer[LineCounter].data)) {
      SerialPort.print("Error loading the Program to LSM6DSOX at line: ");
      SerialPort.println(LineCounter);
      while(1) {
        // Led blinking.
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
      }
    }
  }

  SerialPort.println("Program loaded inside the LSM6DSOX MLC");

  //Interrupts.
  pinMode(INT_1, INPUT);
  attachInterrupt(INT_1, INT1Event_cb, RISING);

  /* We need to wait for a time window before having the first MLC status */
  delay(3000);

  AccGyr.Get_MLC_Output(mlc_out);
  printMLCStatus(mlc_out[0]);
}

void loop() {
  if (mems_event) {
    mems_event=0;
    LSM6DSOX_MLC_Status_t status;
    AccGyr.Get_MLC_Status(&status);
    if (status.is_mlc1) {
      uint8_t mlc_out[8];
      AccGyr.Get_MLC_Output(mlc_out);
      printMLCStatus(mlc_out[0]);
    }
  }
}

void INT1Event_cb() {
  mems_event = 1;
}

void printMLCStatus(uint8_t status) {
  switch(status) {
    case 0:
      SerialPort.println("Activity: Stationary");
      break;
    case 1:
      SerialPort.println("Activity: Walking");
      break;
    case 4:
      SerialPort.println("Activity: Jogging");
      break;
    case 8:
      SerialPort.println("Activity: Biking");
      break;
    case 12:
      SerialPort.println("Activity: Driving");
      break;
    default:
      SerialPort.println("Activity: Unknown");
      break;
  }	  
}
