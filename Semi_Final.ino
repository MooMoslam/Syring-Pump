#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>
#include <AccelStepper.h>

// إعدادات شاشة LiquidCrystal I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2

// إعدادات الـ Keypad
const byte ROW_NUM = 4;    // عدد الصفوف
const byte COLUMN_NUM = 4; // عدد الأعمدة

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6};  // دبابيس الصفوف
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2};  // دبابيس الأعمدة

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// إعدادات محرك الخطوات (Stepper Motor)
const int stepPin = 11;    // دبوس الخطوات
const int dirPin = 10;     // دبوس الاتجاه

AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

// متغيرات لعدد الخطوات
int steps = 0;
int speed = 0;  // السرعة المختارة من 0 إلى 9
int direction; // 1 للدوران في الاتجاه الأمامي، -1 للدوران في الاتجاه العكسي

//buzzer
const int buz = 12;

void setup() {
  pinMode(buz, OUTPUT);

  lcd.init(); // initialize the lcd
  lcd.backlight();

  // إعداد محرك الخطوات
  stepper.setMaxSpeed(1000);  // تعيين السرعة القصوى
  stepper.setAcceleration(500); // تعيين التسارع
  
  // عرض رسالة الترحيب
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Mini_Project");
  lcd.setCursor(2, 1);
  lcd.print("Syringe Pump");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("Press A to");  
  lcd.setCursor(0, 1);  
  lcd.print("set your setting");  
}

void loop() {
  // قراءة الضغطات من لوحة المفاتيح
  char key = keypad.getKey();

  if (key == 'A') {
    steps = 0;
    speed = 0;
    direction = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Steps: ");
  }

  if (key >= '0' && key <= '9') {
    // إذا كان الرقم مدخلًا، نقوم بتحديث عدد الخطوات
    steps = steps * 10 + (key - '0'); // تكوين العدد المدخل
    lcd.setCursor(8, 1);
    lcd.print(steps);
  } 
  else if (key == '#') {
    // إذا تم الضغط على "#" نبدأ تشغيل المضخة
    if (steps == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Please, Enter");
      lcd.setCursor(0, 1);
      lcd.print("steps first !!");
      tone(buz, 1000);  // تشغيل الـ Buzzer بتردد 1000 هرتز
      delay(2000);  // الانتظار قليلاً ليتمكن المستخدم من رؤية الرسالة
      noTone(buz);      // إيقاف الـ Buzzer 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Steps: ");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Speed: ");
      
      // الانتظار لإدخال السرعة
      while (true) {
        char speedKey = keypad.getKey();
        if (speedKey >= '0' && speedKey <= '9') {
          speed = speed * 10 + (speedKey - '0');  // تعيين السرعة بناءً على المدخل
          lcd.setCursor(7, 0);
          lcd.print(speed);  // عرض السرعة المدخلة
          delay(200);  // تأخير بسيط للسماح للمستخدم بمراجعة المدخلات
        } 
        else if (speedKey == '#') {
          // إذا تم الضغط على "#" لبدء تشغيل المضخة
          if (speed < 1 || speed > 1000) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Speed invalid!");
            tone(buz, 1000);  // تشغيل الـ Buzzer بتردد 1000 هرتز
            delay(2000);  // الانتظار قليلاً ليتمكن المستخدم من رؤية الرسالة
            noTone(buz);      // إيقاف الـ Buzzer 
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Speed (1-1000): ");
            speed = 0;  // إعادة تعيين السرعة لتتمكن من إدخالها مجددًا
            lcd.setCursor(0, 1);
            lcd.print(speed);  // عرض السرعة المدخلة
            delay(200);  // تأخير بسيط للسماح للمستخدم بمراجعة المدخلات
          } else {
            lcd.clear();
            lcd.print("Forword: C");
            lcd.setCursor(0,1);
            lcd.print("Backward: D");
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Direction: ");
            // الانتظار لإدخال الاتجاه
            while (true) {
              char directionKey = keypad.getKey();
              if (directionKey == 'C') {
                direction = 1;  // تعيين الاتجاه للأمام
                lcd.print(directionKey);
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print("Forward...");
                delay(2000);
                break;
              }
              else if (directionKey == 'D') {
                direction = -1;  // تعيين الاتجاه للخلف
                lcd.print(directionKey);
                lcd.clear();
                lcd.setCursor(0, 1);
                lcd.print("Backward...");
                delay(2000);
                break;
              }
            }
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Running Pump");
            // تعيين السرعة بناءً على القيمة المدخلة
            stepper.setMaxSpeed(speed);  // تعيين السرعة بناءً على المدخل (من 0 إلى 1000)
            
            // تعيين الاتجاه بناءً على المدخل:
            if (direction == 1) {
              digitalWrite(dirPin, LOW); // تعيين الاتجاه للأمام
            } 
             if (direction == -1){
              digitalWrite(dirPin, HIGH);  // تعيين الاتجاه للخلف
            }
            
            runSyringePump(steps);  // تمرير عدد الخطوات للمضخة
            break;
          }
        }
      }
    }
  }
  else if (key == '*') {
    steps = 0;
    speed = 0;
    // إذا تم الضغط على "*"، نقوم بإلغاء الإدخال وإعادة البدء
    if (steps == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter steps first!");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter steps:");
      lcd.setCursor(13, 1);
      lcd.print(steps);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter steps:");
      lcd.setCursor(13, 1);
      lcd.print(steps);
    }
  }
}

void runSyringePump(long steps) {
  // تشغيل محرك الخطوات وفقًا لعدد الخطوات المدخل
  stepper.moveTo(steps);  // تحديد عدد الخطوات المطلوب تنفيذها
  
  // تنفيذ الحركة
  while (stepper.distanceToGo() != 0) {
    stepper.run();  // تحديث الحركة
  }

  // إظهار رسالة انتهاء العملية
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Done!");
  tone(buz, 1000);  // تشغيل الـ Buzzer بتردد 1000 هرتز
  delay(1000);      // تشغيل الصوت لمدة 1 ثانية
  noTone(buz);      // إيقاف الـ Buzzer 
  steps = 0;
  speed = 0;
  direction = 0;
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("Press A to");  
  lcd.setCursor(0, 1);  
  lcd.print("set your setting");  
}



