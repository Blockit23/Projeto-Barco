#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4     // sensor de temperatura
#define TDS_PIN 34         // sensor TDS/condutividade

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temperatura = 25.0;  // temperatura base

void setup() {
  Serial.begin(9600);
  sensors.begin();
}

void loop() {

  tempzinha();
  cuzin();

  Serial.println("-----------------------------");
  delay(1000);
}

void tempzinha() {
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  if (t > -100) temperatura = t;

  Serial.print("Temperatura: ");
  Serial.println(temperatura);
}

void cuzin() {

  //fica girando 30x igual piru maluco
  int soma = 0;
  for (int i = 0; i < 30; i++) {
    soma += analogRead(TDS_PIN);
    delay(5);
  }

  float media = soma / 30.0;

  // vira tesão
  float voltagem = media * (3.3 / 4095.0);

  
  float ec = (133.42 * voltagem * voltagem * voltagem
            - 255.86 * voltagem * voltagem
            + 857.39 * voltagem);  // coloca em µS/cm

  // arruma com a temp
  float ecComp = ec / (1 + 0.02 * (temperatura - 25.0));

  Serial.print("Condutividade: ");
  Serial.print(ecComp);
  Serial.println(" uS/cm");
}
