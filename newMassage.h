void handleNewMessages(int numNewMessages){

  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++){
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/start"){
      String welcome = "Selamat datang "+ from_name + " di Bot Monitoring kWh .\n";
      welcome += "Ketik /help untuk menegtahui fungsi dari perintah.";
      String keyboardJson = "[[\"/cekTegangan\", \"/cekArus\",\"/cekDaya\",\"/cekSuhu\"],[\"/status\",\"/dashboard\",\"/nyala\",\"/help\"],[\"/setBatas\",\"/batas\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id,welcome,"", keyboardJson, true);
      return;
    }
    if (text == "/help"){
      String help = "Berikut perintah yang dapat digunakan di Bot Monitoring IoT : \n\n";
      help += "/ledon : Untuk menyalakan LED\n";
      help += "/ledoff : Untuk Mematikan LED\n";
      help += "/cekTegangan: Menampilkan status Tegangan\n";
      help += "/cekArus : Menampilkan status Arus\n";
      help += "/cekDaya : Menampilkan status Daya\n";
      help += "/cekSuhu : Menampilkan status Suhu\n";
      help += "/status : Menampilkan status Tegangan, Arus, Daya dan Suhu\n";
      help += "/dashboard: Menampilkan Dashboard Monitoring\n";
      help += "/nyala : Mengaktifkan kembali relay\n";
      help += "/batas : Mengaktifkan relay\n";
      help += "/setBatas : Mengaktifkan kembali relay\n";
      bot.sendMessage(chat_id, help, "Markdown");
      return;
    }
    if (text == "/status"){
      String val_tegangan = String (Voltage,1);
      String val_arus = String (Current,1);
      String val_daya = String (Power,1);
      String val_suhu = String (Temp,1);

      String Status ="Status Tegangan, Arus, Daya dan Suhu saat ini:\n";
      Status += "Tegangan : "+ val_tegangan + " V\n";
      Status += "Arus : "+ val_arus + " A\n";
      Status += "Daya : "+ val_daya + " W\n";
      Status += "Suhu : "+ val_suhu + " °C\n";
      bot.sendMessage(chat_id, Status, "Markdown");
      return;
      
    }
    if (text == "/ledon"){
      digitalWrite(ledPin, HIGH); // turn the LED on (HIGH is the voltage level)
      bot.sendMessage(chat_id, "Led is ON", "");
      return;
    }

    if (text == "/ledoff"){
      digitalWrite(ledPin, LOW); // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
      return;
    }

    if (text == "/cekTegangan"){
      String volt = "Tegangan saat ini ";
      volt += String (Voltage,2);
      volt += " V";
      bot.sendMessage(chat_id, volt, "");
      return;
    }
    if (text == "/cekArus"){
      String current = "Arus saat ini ";
      current += String (Current,2);
      current += " A";
      bot.sendMessage(chat_id, current, "");
      return;
    }
    if (text == "/cekDaya"){
      String pwr = "Daya saat ini ";
      pwr += String (Power,2);
      pwr += " W";
      bot.sendMessage(chat_id, pwr, "");
      return;
    }
    if (text == "/cekSuhu"){
      String temperature = "Suhu Box saat ini ";
      temperature += String (Temp,2);
      temperature += " °C";
      bot.sendMessage(chat_id, temperature, "");
      return;
    }
    if (text == "/dashboard"){
      String dashboard = "https://backend.thinger.io/console/dashboards/ESP32_IoT";
      bot.sendMessage(chat_id, dashboard, "");
      return;
    }
    if (text == "/batas"){
      relayState = true;
      String State = "Pembatas Aktif";
      bot.sendMessage(chat_id, State, "");
      return;
    }
    if (text == "/nyala"){
      relayState = false;
      notifState = false;
      String RState = "Relay aktif kembali";
      bot.sendMessage(chat_id, RState, "");
      return;
    }
    if (text == "/setBatas"){
      String setbatas = "Masukan batas Kwh\n";
      bot.sendMessage(chat_id, setbatas, "");
      botState=1;
      return;
    }
    if (text != ""){
      msgState = 1;
      return;
    }
    if (botState == 1&& msgState == 1){
      String batas = text;
      batasCut = batas.toInt();
      String txt = "Batas kWh sudah di perbarui.";
      bot.sendMessage(chat_id, txt, "");
      botState = 0;
      msgState = 0;
      return;
    }
  }
}
