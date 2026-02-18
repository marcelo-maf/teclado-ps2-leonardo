 /* * Desenvolvedor: Marcelo (marcelorbpi@gmail.com)

 * Data: 17/02/2026 - Versão 1.6 (Restauração da 1.1 + Fix da Barra)

 * horario 00:32

 */


#include <Keyboard.h>

#include <HID.h> // Necessário para a função de injeção bruta


// --- ESTRUTURA PARA ENVIO BRUTO (FIX PARA BARRA \ NO PI) ---

typedef struct {

  uint8_t modifiers;

  uint8_t reserved;

  uint8_t keys[6];

} KeyReport_t;


void enviarTeclaBruta(uint8_t tecla, bool shift) {

  KeyReport_t report;

  report.reserved = 0;

  report.modifiers = shift ? 0x02 : 0x00; // Shift Esquerdo

  for (int i = 0; i < 6; i++) report.keys[i] = 0;

  report.keys[0] = tecla;

  HID().SendReport(2, &report, sizeof(KeyReport_t));

}


void liberarTudoBruto() {

  KeyReport_t report = {0, 0, {0,0,0,0,0,0}};

  HID().SendReport(2, &report, sizeof(KeyReport_t));

}

// ---------------------------------------------------------


const int DataPin = 2;

const int IRQpin = 3;


volatile uint8_t scanCode = 0;

volatile bool dataReady = false;

volatile int bitCount = 0;

bool shiftAtivo = false;


void readBit() {

  int val = digitalRead(DataPin);

  if (bitCount >= 1 && bitCount <= 8) {

    scanCode |= (val << (bitCount - 1));

  }

  bitCount++;

  if (bitCount == 11) {

    dataReady = true;

    bitCount = 0;

  }

}


void setup() {

  pinMode(DataPin, INPUT_PULLUP);

  pinMode(IRQpin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(IRQpin), readBit, FALLING);

  Keyboard.begin();

}


void loop() {

  if (dataReady) {

    uint8_t code = scanCode;

    dataReady = false;

    scanCode = 0;


    static bool skipNext = false;

    static bool estendida = false;


    if (code == 0xE0) { estendida = true; return; }

    if (code == 0xF0) { skipNext = true; return; }


    if (skipNext) {

      if (code == 0x12 || code == 0x59) {

        shiftAtivo = false;

        Keyboard.release(code == 0x12 ? KEY_LEFT_SHIFT : KEY_RIGHT_SHIFT);

      }

      if (code == 0x14) Keyboard.release(KEY_LEFT_CTRL);

      

      liberarTudoBruto(); // Garante soltar a barra bruta

      skipNext = false;

      return;

    }


    if (estendida && (code == 0x12 || code == 0x59)) { return; }


    switch (code) {

      // SETAS DE NAVEGAÇÃO (Recuperadas!)

      case 0x75: Keyboard.write(KEY_UP_ARROW); break;    

      case 0x72: Keyboard.write(KEY_DOWN_ARROW); break;  

      case 0x6B: Keyboard.write(KEY_LEFT_ARROW); break;  

      case 0x74: Keyboard.write(KEY_RIGHT_ARROW); break; 


      // CONTROLE

      case 0x12: Keyboard.press(KEY_LEFT_SHIFT); shiftAtivo = true; break;

      case 0x59: Keyboard.press(KEY_RIGHT_SHIFT); shiftAtivo = true; break;

      case 0x14: Keyboard.press(KEY_LEFT_CTRL); break;

      case 0x5A: Keyboard.write(KEY_RETURN); break;

      case 0x66: Keyboard.write(KEY_BACKSPACE); break;

      case 0x29: Keyboard.write(' '); break;

      case 0x0D: Keyboard.write(KEY_TAB); break; 


      // E-MAIL (F1)

      case 0x05: 

        Keyboard.releaseAll(); 

        delay(10);

        Keyboard.print("marcelorbpi@gmail.com"); 

        if(shiftAtivo) Keyboard.press(KEY_LEFT_SHIFT);

        break;


      // ALFABETO

      case 0x1C: Keyboard.write('a'); break; case 0x32: Keyboard.write('b'); break;

      case 0x21: Keyboard.write('c'); break; case 0x23: Keyboard.write('d'); break;

      case 0x24: Keyboard.write('e'); break; case 0x2B: Keyboard.write('f'); break;

      case 0x34: Keyboard.write('g'); break; case 0x33: Keyboard.write('h'); break;

      case 0x43: Keyboard.write('i'); break; case 0x3B: Keyboard.write('j'); break;

      case 0x42: Keyboard.write('k'); break; case 0x4B: Keyboard.write('l'); break;

      case 0x3A: Keyboard.write('m'); break; case 0x31: Keyboard.write('n'); break;

      case 0x44: Keyboard.write('o'); break; case 0x4D: Keyboard.write('p'); break;

      case 0x15: Keyboard.write('q'); break; case 0x2D: Keyboard.write('r'); break;

      case 0x1B: Keyboard.write('s'); break; case 0x2C: Keyboard.write('t'); break;

      case 0x3C: Keyboard.write('u'); break; case 0x2A: Keyboard.write('v'); break;

      case 0x1D: Keyboard.write('w'); break; case 0x22: Keyboard.write('x'); break;

      case 0x35: Keyboard.write('y'); break; case 0x1A: Keyboard.write('z'); break;


      // NÚMEROS

      case 0x45: Keyboard.write('0'); break; case 0x16: Keyboard.write('1'); break;

      case 0x1E: Keyboard.write('2'); break; case 0x26: Keyboard.write('3'); break;

      case 0x25: Keyboard.write('4'); break; case 0x2E: Keyboard.write('5'); break;

      case 0x36: Keyboard.write('6'); break; case 0x3D: Keyboard.write('7'); break;

      case 0x3E: Keyboard.write('8'); break; case 0x46: Keyboard.write('9'); break;


      // SÍMBOLOS

      case 0x4C: Keyboard.write(';'); break;

      case 0x54: Keyboard.write('['); break;

      case 0x5B: Keyboard.write(']'); break;

      case 0x5D: Keyboard.write(0x5C); break;

      case 0x52: Keyboard.write('\''); break;

      case 0x4A: Keyboard.write('/'); break;


      // --- A BARRA INVERTIDA (0x61) - AGORA USANDO BYPASS ---

      case 0x61: 

        Keyboard.releaseAll(); 

        // 100 é o Usage ID para a tecla da Barra Invertida ABNT2

        enviarTeclaBruta(100, shiftAtivo); 

        delay(5);

        liberarTudoBruto();

        break;


      case 0x51: 

        if (shiftAtivo) {

          Keyboard.press(KEY_RIGHT_ALT); 

          Keyboard.write('w');

          Keyboard.release(KEY_RIGHT_ALT);

        } else {

          Keyboard.write(0xDC); 

        }

        break;


      case 0x4E: Keyboard.write('-'); break;

      case 0x55: Keyboard.write('='); break;

      case 0x41: Keyboard.write(','); break;

      case 0x49: Keyboard.write('.'); break;

    }

    

    estendida = false;

  }

} 
