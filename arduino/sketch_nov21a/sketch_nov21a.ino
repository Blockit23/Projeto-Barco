/*
  ARQUIVO DO ESP
  Data de criação: 26/11/2025
  Data da ultima atualização: 03/12/2025
  Contém: envido de dados de temperatura, condutividade e status. 
*/

#include <OneWire.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DallasTemperature.h>

const char* ssid = "";    //Nome do WiFi
const char* password = "";     //Senha do wifi

#define API_KEY ""  //poe a chave da api aqui
#define DATABASE_URL "https://seu-projeto.firebaseio.com/"  //poe a url certa

FirebaseData fbData;
FirebaseAuth auth;
FirebaseConfig config;

#define ONE_WIRE_BUS 4  //sensor de temperatura
#define TDS_PIN 34      //sensor de condutividade

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temperatura;
int erro = 0;
String statusTemp = "OK";
String statusCondutividade = "OK";

//===============================================FUNÇÃO DE CONEXÃO=============================================================
bool conectarWifi() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int tentativas = 0;

  while (WiFi.status() != WL_CONNECTED && tentativas < 100) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado ao Wi-Fi!");
    Serial.print(" IP do ESP32: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("\nFalha ao conectar ao Wi-Fi!");
    Serial.print("Verifique o nome a senha");
    return false;
  }
}

//==========================================================MEDIR TEMPERATURA====================================================
void medirTemperatura() {
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);

  if (t > -100 && t < 200) {
    temperatura = t;
  } else {
    Serial.print("Erro no sensor de temperatura");
    statusTemp = "erro";
    Serial.println(t);
  }

  if (temperatura < 18 || temperatura > 25) {
    Serial.print("ALERTA: Temperatura fora da faixa esperada!");
    statusTemp = "Alerta";
    erro++;

    if (erro > 3) {
      Serial.println("Possível erro no sensor de temperatura. Caso o erro persista, procure manutenção");
    }
    return;
  }

  Serial.print("Temperatura: ");
  Serial.println(temperatura);
}

//=============================================================CONDUTIVIDADE=========================================================
float medirCondutividade() {
  int soma = 0; 

  for (int i = 0; i < 30; i++) {
    int leitura = analogRead(TDS_PIN);

    if (leitura < 0 || leitura > 4095) {
      statusCondutividade = "ERRO: leitura invalida";
      return -1;
    }

    soma += leitura;
    delay(500);
  }

  float media = soma / 30.0;

  if (media < 10) {
    statusCondutividade = "ERRO: sensor desconectado";
    return -1;
  }

  float voltagem = media * (3.3 / 4095.0);

  float ec = (133.42 * voltagem * voltagem * voltagem
            - 255.86 * voltagem * voltagem
            + 857.39 * voltagem);

  float ecComp = ec / (1 + 0.02 * (temperatura - 25.0));

  if (ecComp < 0) {
    statusCondutividade = "ERRO: valor negativo";
  }
  else if (ecComp > 2000) {
    statusCondutividade = "ALERTA: condutividade alta";
  }
  else {
    statusCondutividade = "OK";
  }

  Serial.print("Condutividade: ");
  Serial.print(ecComp);
  Serial.print(" uS/cm | Status: ");
  Serial.println(statusCondutividade);

  return ecComp;
}

//=============================================================ENVIO DE DADOS========================================================

void enviaDados(float temp, float cond, String statusTemp, String statusCond) {

  if (!Firebase.RTDB.setFloat(&fbData, "/sensores/temperatura", temp))
    Serial.println(fbData.errorReason());

  if (!Firebase.RTDB.setFloat(&fbData, "/sensores/condutividade", cond))
    Serial.println(fbData.errorReason());

  if (!Firebase.RTDB.setString(&fbData, "/sensores/statusTemp", statusTemp))
    Serial.println(fbData.errorReason());

  if (!Firebase.RTDB.setString(&fbData, "/sensores/statusCond", statusCond))
    Serial.println(fbData.errorReason());
}

void setup() {
  Serial.begin(9600);

  sensors.begin();

  if (!conectarWifi()) {
    Serial.println("Não foi possível conectar-se a internet, verifique sua conexão com a internet!");
    Serial.println("Caso o problema persista entre em contato com nosso suporte");
  }
  
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);

  Serial.println("Firebase conectado!");

  delay(1000);

}

void loop() {
  medirTemperatura();
  float ec = medirCondutividade();

  enviaDados(temperatura, ec, statusTemp, statusCondutividade);

  delay(2000);
}
