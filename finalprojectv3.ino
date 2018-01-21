int pin=10;
int ipin2=2;
int ipin3=3;

volatile int state=HIGH; //state of light
volatile int count=0; 
volatile int overflowCount=0; //counter for overflows
volatile bool checkCode=0; 

bool left= false; //if comes from left
bool right=false; //if comes from right
volatile int val2=0; //for reading pin  2
volatile int val3=0; //for reading pin 3


void setup() {
  Serial.begin(9600);
  cli(); //clears interrupts
  pinMode(pin, OUTPUT);
  TCCR1A=0; //timer counter countrol register 1A
  TCCR1B=0; //timer counter countrol register 1 B
  TCCR1B=(1<<2); // set FREQUENCY TO CLK/256
  TIMSK1 |=(7<<0); //Enable OCR interrupts

  OCR1A=100000;//calls ISR when timer counts to 100000
  OCR1B=100001;// calls ISR when timer counts to 100001
  attachInterrupt(0,handler,CHANGE); //interrupt (pin 2, calls handler, on change)
  attachInterrupt(1, handler, CHANGE);// interrupt (pin 3, calls handler, on change)
  sei(); //sets interrupts
}

ISR (TIMER1_COMPA_vect){
  overflowCount += 1; 
  if (overflowCount==10){ //time limit is 7 seconds
      Serial.println("Time limit reached.");
      checkCode=1; //timer has reached time limit
      overflowCount=0;
  }
}

ISR (TIMER1_COMPB_vect){
  TCNT1=0; //resets timer 
}

void handler(){
state=!state;
count=count+1;
val2=digitalRead(ipin2);
val3=digitalRead(ipin3);


if(count==1){//only digitalReads on the first interrupt
  if (val2==HIGH && val3 ==LOW){
    left=true;
    right=false;
  }
  else if (val2==LOW && val3 ==HIGH){
    right=true;
    left=false;
  }
}

Serial.println("Count is:");
Serial.println(count);
Serial.println("");
}


void loop() {
  digitalWrite(pin,state);
  if (checkCode==1){ //timer has reached its limit so need to check passcode
    Serial.println("checkcode is 1");
    Serial.println("");
    TCCR1B=0;//stops timer
    checkCode=0;  //resets checkCode to 0
    Serial.println("Left value is :");
    Serial.println(left);
    Serial.println("");
    if (count>=12 && count<=16 && left==1 ){//range of interrupts that are the correct passcode, and comes from left or right
      Serial.println("Passcode correct. Unlocking door.");
      count=0;
    }
    else{
      Serial.println("Passcode incorrect.Re-try.");
      count=0;
    }
  TCCR1B=(1<<2); //resumes timer
  }
  //left=false;
  //right=false;
}
