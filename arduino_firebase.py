import serial
import firebase_admin
from firebase_admin import credentials, db
import time
import re

CAMINHO_CHAVE = r"C:/Putaria/firebase-admin.json"
URL_DATABASE = 'https://barco-teste-default-rtdb.firebaseio.com/'
PORTA_ESP = 'COM5'
BAUD_RATE = 9600

# Conecta a fire
try:
    cred = credentials.Certificate(CAMINHO_CHAVE)
    firebase_admin.initialize_app(cred, {'databaseURL': URL_DATABASE})
    print("Firebase conectado com sucesso!")
except Exception as e:
    print("Erro ao conectar ao Firebase:", e)
    exit()

# Conecta dst
try:
    esp32 = serial.Serial(PORTA_ESP, BAUD_RATE)
    print(f"Conectado ao ESP32 na porta {PORTA_ESP}")
except Exception as e:
    print("Erro ao conectar ao ESP32:", e)
    exit()

# começa o roda roda gira
while True:
    try:
        dado = esp32.readline().decode(errors="ignore").strip()
        print("ESP32:", dado)

        # transforma em numero
        match = re.search(r"[-+]?\d*\.\d+|\d+", dado)
        if match:
            temperatura = float(match.group())

            # Envia para o Firebase
            db.reference("arduino/temperatura").set(temperatura)
            print(f"Temperatura enviada: {temperatura}")

        time.sleep(1)

    except KeyboardInterrupt:
        print("Encerrando programa...")
        break

    except Exception as e:
        print("Erro no loop:", e)
