#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#include <SoftwareSerial.h>

WiFiServer servidor(80);
String html;//String que armazena o corpo do site.
String sJsonReturnEsp;

SoftwareSerial ESP8266Serial(D6, D7);

void setup()
{
   Serial.begin(9600);
   ESP8266Serial.begin(9600);

   WiFi.mode(WIFI_STA);
   WiFi.begin("sitnet_0968", "xa39by93");
   while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
   }
   Serial.println("IP: ");
   Serial.println(WiFi.localIP());
   servidor.begin();
}

void loop()
{
   getContentSerialEsp();
   http();
}
void getContentSerialEsp() {
  // int i = 0;    
  // while (i < 100) {
    while (ESP8266Serial.available()) {
        delay(1);
        char c = ESP8266Serial.read();
        sJsonReturnEsp += String(c);
    }
//   i++;
//  }
}
void http()
{
   WiFiClient cliente = servidor.available();
   delay(100);
   if (cliente.connected()) {
      String req = cliente.readStringUntil('\r');
      Serial.println(req);
      html = "";
      html += "HTTP/1.1 Content-Type: text/html\n\n";
      html += "<!DOCTYPE html><html><head><title>ADM - BKP</title>";
      html += "<meta name='viewport' content='user-scalable=no'>";
      html += "<meta charset='UTF-8'/>";
      html += "<style>h1{font-size:2vw;color:black;}</style></head>";
      html += "<body bgcolor='ffffff'><center>";
      
      if (req.indexOf("/reiniciar") > -1) {
        html += "<h1>Reiniciando máquina</h1>";
        html += "<br><br>";
        html += "<p>Comando enviado para o computador cliente</p>";
        html += "<form action='/?action=1' method='get'>";
        html += "<input type='submit' value='Voltar para a página inicial' id='frm1_submit'/></form>";           
        html += "<br><br>";
        ESP8266Serial.write("reiniciar");
      } else if (req.indexOf("/download") > -1) {
            // @TODO RETIRAR ISSO AQUI
            sJsonReturnEsp = "ini:dsdsdsds:fim";
            if (sJsonReturnEsp.indexOf("ini:") > -1 && sJsonReturnEsp.indexOf(":fim")) {
                sJsonReturnEsp.replace("ini:", "");
                sJsonReturnEsp.replace(":fim", "");
                int str_len = sJsonReturnEsp.length() + 1; 
                char char_array[str_len];
                
                cliente.println("HTTP/1.1 200 OK");
                cliente.println("Content-Type: octet-stream");
                cliente.println("Content-Length: " + String(str_len));
                cliente.println("Content-Disposition: attachment; filename=bkp.txt;");  
                cliente.println("Content-Type: application/force-download");
                cliente.println();

                
                sJsonReturnEsp.toCharArray(char_array, str_len);
                cliente.write(char_array, str_len);
                sJsonReturnEsp = "";       
                cliente.flush();
            } else {
              html += "<h1>Arquivo não criado ainda, tente novamente...</h1>";
              html += "<br><br>";        
              html += "<form action='/download?action=1' method='get' target='_BLANK'>";
              html += "<p id='js-contagem'>30</p>";
              html += "<input type='submit' style='display:none;' value='Download' id='frm1_submit'/></form>";          
              html += "<br><br>";
              html += "<form action='/?action=1' method='get'>";
              html += "<input type='submit' value='Voltar' id='frm1_submit'/></form>";          
              html += "<br><br>";
              html += "<script> var iInterval = setInterval(function() {if(+document.getElementById('js-contagem').innerHTML == 0) {clearInterval(iInterval); document.getElementById('js-contagem').style.display = 'none'; document.getElementById('frm1_submit').style.display = 'block'; return;} document.getElementById('js-contagem').innerHTML = +(document.getElementById('js-contagem').innerHTML) - 1;}, 1000);</script>";        
            }
      } else if (req.indexOf("/bkp") > -1) {
        html += "<h1>Comando enviado, aguarde um instante até o arquivo ser criado</h1>";
        html += "<br><br>";        
        html += "<form action='/download?action=1' method='get' target='_BLANK'>";
        html += "<p id='js-contagem'>30</p>";
        html += "<input type='submit' style='display:none;' value='Download' id='frm1_submit'/></form>";          
        html += "<br><br>";
        html += "<form action='/?action=1' method='get'>";
        html += "<input type='submit' value='Voltar' id='frm1_submit'/></form>";          
        html += "<br><br>";
        html += "<script> var iInterval = setInterval(function() {if(+document.getElementById('js-contagem').innerHTML == 0) {clearInterval(iInterval); document.getElementById('js-contagem').style.display = 'none'; document.getElementById('frm1_submit').style.display = 'block'; return;} document.getElementById('js-contagem').innerHTML = +(document.getElementById('js-contagem').innerHTML) - 1;}, 1000);</script>";        
        ESP8266Serial.write("bkp");
      } else {
        html += "<h1>Página inicial</h1>";
        html += "<br><br>";
        html += "<form action='/reiniciar?action=1' method='get'>";
        html += "<input type='submit' value='Reiniciar PC' id='frm1_submit'/></form>";          
        html += "<br><br>";
        html += "<form action='/bkp?action=1' method='get'>";
        html += "<input type='submit' value='Download do BKP' id='frm1_submit'/></form>";          
      }
      html += "</center></body></html>";
      cliente.print(html);
      cliente.stop();
   }
}
