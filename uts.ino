// =======================================================
// === SKETCH ARDUINO: SMART CONVEYOR SPEED CONTROL (HANYA MOTOR DC) ===
// === Menerima data serial: PWM (0-255), DELAY (ms), CMD (1/2) ===
// === FOKUS: Mengendalikan kecepatan motor berdasarkan nilai PWM dari Fuzzy Logic di Python ===
// =======================================================

// --- PIN KONFIGURASI MOTOR KONVEYOR UTAMA (L298N Motor Driver) ---
const int MOTOR_PWM_PIN = 9;   // Pin PWM (analogWrite) -> ENB Motor Driver (Kecepatan)
const int MOTOR_IN3 = 7;       // Pin Digital -> IN3 Motor Driver (Arah Maju)
const int MOTOR_IN4 = 8;       // Pin Digital -> IN4 Motor Driver (Arah Mundur/Stop)

// --- VARIABEL GLOBAL ---
String receivedData;
int motorPWM = 0;       // Diperbarui oleh Fuzzy Logic dari Python (0-255)
int sortDelayMs = 0;    // Parsed, tapi tidak digunakan
int sortCommand = 0;    // Parsed, tapi tidak digunakan

void setup() {
  // Inisialisasi Serial (Harus sama dengan BAUD_RATE di Python: 9600)
  Serial.begin(9600); 
  
  // Set pin motor konveyor sebagai OUTPUT
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  
  // Tetapkan Arah Konveyor: Maju (Sesuaikan jika Anda menggunakan IN3/IN4)
  digitalWrite(MOTOR_IN3, HIGH); 
  digitalWrite(MOTOR_IN4, LOW);  
  
  // Motor mulai dengan kecepatan awal sangat rendah (atau 0)
  analogWrite(MOTOR_PWM_PIN, 50);  // Kecepatan awal rendah
  
  Serial.println("Arduino Siap. Menunggu data kontrol kecepatan dari Python...");
}

void loop() {
  // 1. PENERIMAAN DATA SERIAL (Non-blocking)
  if (Serial.available() > 0) {
    receivedData = Serial.readStringUntil('\n');
    
    // Parsing data: PWM,DELAY,CMD
    int firstCommaIndex = receivedData.indexOf(',');
    int secondCommaIndex = receivedData.indexOf(',', firstCommaIndex + 1);

    if (firstCommaIndex != -1 && secondCommaIndex != -1) {
      // Nilai pertama adalah PWM (Kecepatan)
      motorPWM = receivedData.substring(0, firstCommaIndex).toInt();
      // Nilai kedua (Delay) dan ketiga (Command) tetap diparse meskipun tidak digunakan untuk motor
      sortDelayMs = receivedData.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
      sortCommand = receivedData.substring(secondCommaIndex + 1).toInt();
      
      // Batasi nilai PWM yang diterima agar aman (0 hingga 255)
      motorPWM = constrain(motorPWM, 0, 255);
      
      Serial.print("Menerima PWM (Kecepatan): ");
      Serial.println(motorPWM);
    } else {
      Serial.println("ERROR: Format data serial salah.");
    }
  }

  // 2. KONTROL KECEPATAN KONVEYOR (EKSEKUSI FUZZY LOGIC)
  // Ini adalah baris KUNCI. PWM yang dihitung oleh Fuzzy Logic di Python
  // diterapkan langsung ke motor.
  // Kecepatan motor DITENTUKAN oleh tingkat keyakinan YOLO!
  analogWrite(MOTOR_PWM_PIN, motorPWM);

  delay(10); // Delay kecil agar loop berjalan lancar
}