/* * Projeto: Conversor de Teclado PS/2 para USB (HID)
 * Desenvolvedor: Marcelo (marcelorbpi@gmail.com)
 * Data: 16/02/2026
 * Versão: 1.1 (Estabilidade de Setas e Documentação)
 */

#include <Keyboard.h> 

const int DataPin = 2; // Pino de dados do teclado
const int IRQpin = 3;  // Pino de Clock (Interrupção) do teclado

volatile uint8_t scanCode = 0;   // Armazena o byte bruto recebido
volatile bool dataReady = false; // Flag: indica que um byte foi processado
volatile int bitCount = 0;      // Contador para os 11 bits do protocolo PS/2
bool shiftAtivo = false;        // Monitora se Shift está pressionado

// Função de Interrupção: Captura os bits conforme o Clock do teclado cai (FALLING)
void readBit() {
  int val = digitalRead(DataPin);
  if (bitCount >= 1 && bitCount <= 8) {
    scanCode |= (val << (bitCount - 1)); // Reconstrói o byte bit a bit
  }
  bitCount++;
  if (bitCount == 11) { // Ao completar 11 bits, sinaliza leitura pronta
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

    // 1. Detecta prefixo de tecla estendida (Setas)
    if (code == 0xE0) { 
      estendida = true; 
      return; 
    }

    // 2. Detecta quando a tecla é solta
    if (code == 0xF0) { 
      skipNext = true; 
      return; 
    }

    // 3. Processa a liberação da tecla
    if (skipNext) { 
      if (code == 0x12 || code == 0x59) {
        shiftAtivo = false;
        Keyboard.release(code == 0x12 ? KEY_LEFT_SHIFT : KEY_RIGHT_SHIFT);
      }
      if (code == 0x14) Keyboard.release(KEY_LEFT_CTRL);
      
      skipNext = false; 
      // Não resetamos 'estendida' aqui para não quebrar a sequência
      return; 
    }

    // 4. Ignora o Shift Falso (0x12) que vem com as setas
    if (estendida && (code == 0x12 || code == 0x59)) {
       return; 
    }

    // 5. Mapeamento das teclas
    switch (code) {
      // SETAS DE NAVEGAÇÃO
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
      case 0x0D: Keyboard.write(KEY_TAB); break; // Adiciona o TAB para o terminal

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
      
      //case 0x51: Keyboard.write(0xDC); break;
      case 0x4E: Keyboard.write('-'); break;
      case 0x55: Keyboard.write('='); break;
      case 0x41: Keyboard.write(','); break;
      case 0x49: Keyboard.write('.'); break;

 case 0x51: 
        if (shiftAtivo) {
          // INTERROGAÇÃO (Já sabemos que este funciona!)
          Keyboard.press(KEY_RIGHT_ALT); 
          Keyboard.write('w');
          Keyboard.release(KEY_RIGHT_ALT);
        } else {
          // BARRA: Vamos tentar o código de tecla que o Pi associa à barra
          // Se o '/' puro falha, tentamos o atalho AltGr + Q de novo ou o ASCII direto
          Keyboard.write(0xDC); // Este é o código HID para a barra '/' em muitos teclados
        }
        break;
    }
    
    // Reseta a flag estendida para a próxima tecla
    estendida = false;
  }
}
