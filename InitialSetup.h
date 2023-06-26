void InitialSetup(){
  lcd.setCursor (0, 0);
  lcd.print("    Welcome   ");
  lcd.setCursor (0, 1);
  lcd.print("   ---Minaa---  ");
  delay(3000);

  emon1.voltage(pinTegangan, 105, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(pinArus, 11.9);       // Current: input pin, calibration.

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  delay(10);
  digitalWrite(ledPin, LOW); // initialize pin as off (active LOW)
  

  digitalWrite(pinRelay1, HIGH);
  digitalWrite(pinRelay2, HIGH);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED)
 {
  lcd.clear();
  int i = 0;
   Serial.print(".");
   lcd.setCursor (0, 0);
   lcd.print("ConnectToWifi");
   lcd.setCursor (0+i, 1);
   lcd.print("      :)");
   i++;
   delay(500);
 }
 secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
 Serial.print("\nWiFi connected. IP address: ");
 Serial.println(WiFi.localIP());

 Serial.print("Retrieving time: ");
 configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
 time_t now = time(nullptr);
 while (now < 24 * 3600)
 {
   Serial.print(".");
   delay(100);
   now = time(nullptr);
 }
 Serial.println(now);

 lcd.clear();

 delay (500);

}
