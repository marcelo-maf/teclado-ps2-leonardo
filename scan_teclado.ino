// SCANNER RAIZ - SEM BIBLIOTECAS
const int dataPin = 2;
const int clockPin = 3;

void setup() {
  pinMode(dataPin, INPUT);
  pinMode(clockPin, INPUT);
  Serial.begin(9600);
  Serial.println("Aperte qualquer tecla (especialmente as 19 extras)...");
}

void loop() {
  // Espera o clock cair (início do sinal PS/2)
  if (digitalRead(clockPin) == LOW) {
    int data = 0;
    // Pula o start bit e lê 8 bits de dados
    for (int i = 0; i < 11; i++) {
      while (digitalRead(clockPin) == LOW);
      while (digitalRead(clockPin) == HIGH);
      if (i > 0 && i < 9) {
        if (digitalRead(dataPin) == HIGH) {
          data |= (1 << (i - 1));
        }
      }
    }
    Serial.print("Scan Code Bruto: 0x");
    Serial.println(data, HEX);
    delay(20); // Evita repetição fantasma
  }
}
