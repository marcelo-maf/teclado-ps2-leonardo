/* * Projeto: Conversor de Teclado PS/2 para USB (HID)
 * Desenvolvedor: Marcelo (marcelorbpi@gmail.com)
 * Data: 16/02/2026
 * Descrição: Mapeamento personalizado de Scan Codes PS/2 para Raspberry Pi (Layout ABNT2/US-Inter)
 */

#include <Keyboard.h> 

// Definição dos pinos de comunicação com o teclado PS/2
const int DataPin = 2; 
const int IRQpin = 3;  

// Variáveis voláteis para processamento dentro da interrupção
volatile uint8_t scanCode = 0;   // Armazena o byte recebido do teclado
volatile bool dataReady = false; // Flag que indica byte completo recebido
volatile int bitCount = 0;      // Contador de bits do protocolo PS/2
bool shiftAtivo = false;        // Controle de estado das teclas Shift

// Função de Interrupção: Lê bit a bit o sinal de clock do teclado
void readBit() {
  int val = digitalRead(DataPin);
  if (bitCount >= 1 && bitCount <= 8) {
    scanCode |= (val << (bitCount - 1)); // Monta o byte (LSB first)
  }
  bitCount++;
  if (bitCount == 11) { // Protocolo PS/2 tem 11 bits por pacote
    dataReady = true;
    bitCount = 0;
  }
}

void setup() {
  pinMode(DataPin, INPUT_PULLUP);
  pinMode(IRQpin, INPUT_PULLUP);
  // Ativa a interrupção no pino de Clock (IRQpin)
  attachInterrupt(digitalPinToInterrupt(IRQpin), readBit, FALLING);
  Keyboard.begin(); // Inicializa a biblioteca de teclado USB
}

void loop() {
  if (dataReady) {
    uint8_t code = scanCode;
    dataReady = false;
    scanCode = 0;
    static bool skipNext = false; // Flag para ignorar o código após o 0xF0 (soltar tecla)

    // Se receber 0xF0, a próxima leitura será o código da tecla sendo solta
    if (code == 0xF0) { skipNext = true; return; }

    if (skipNext) { 
      // Gerencia a liberação das teclas modificadoras
      if (code == 0x12 || code == 0x59) {
        shiftAtivo = false;
        Keyboard.release(code == 0x12 ? KEY_LEFT_SHIFT : KEY_RIGHT_SHIFT);
      }
      if (code == 0x14) Keyboard.release(KEY_LEFT_CTRL);
      skipNext = false; 
      return; 
    }

    // Tradutor de Scan Codes para comandos HID USB
    switch (code) {
      // --- CONTROLE E SISTEMA ---
      case 0x12: Keyboard.press(KEY_LEFT_SHIFT); shiftAtivo = true; break;
      case 0x59: Keyboard.press(KEY_RIGHT_SHIFT); shiftAtivo = true; break;
      case 0x14: Keyboard.press(KEY_LEFT_CTRL); break;
      case 0x5A: Keyboard.write(KEY_RETURN); break;    // Tecla Enter
      case 0x66: Keyboard.write(KEY_BACKSPACE); break; // Tecla Apagar
      case 0x29: Keyboard.write(' '); break;           // Tecla Espaço

      // --- SMART KEY NO F1 (Atalho de E-mail) ---
      case 0x05: 
        Keyboard.releaseAll(); // Evita conflitos de teclas presas
        delay(10);
        Keyboard.print("marcelorbpi@gmail.com"); 
        if(shiftAtivo) Keyboard.press(KEY_LEFT_SHIFT); // Restaura estado do Shift
        break; 

      // --- MAPEAMENTO ALFABÉTICO (Scan Codes Set 2) ---
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

      // --- MAPEAMENTO NUMÉRICO ---
      case 0x45: Keyboard.write('0'); break; case 0x16: Keyboard.write('1'); break;
      case 0x1E: Keyboard.write('2'); break; case 0x26: Keyboard.write('3'); break;
      case 0x25: Keyboard.write('4'); break; case 0x2E: Keyboard.write('5'); break;
      case 0x36: Keyboard.write('6'); break; case 0x3D: Keyboard.write('7'); break;
      case 0x3E: Keyboard.write('8'); break; case 0x46: Keyboard.write('9'); break;

      // --- BLOCO DE SÍMBOLOS AJUSTADO (Layout Físico Marcelo) ---
      case 0x4C: Keyboard.write(';'); break;  // Mapeado para Ç no Pi
      case 0x54: Keyboard.write('['); break;  // Mapeado para Acentos no Pi
      case 0x5B: Keyboard.write(']'); break;  // Mapeado para Abre Colchetes
      case 0x5D: Keyboard.write(0x5C); break; // Ajuste físico: Envia Barra Invertida (\)
      case 0x52: Keyboard.write('\''); break; // Mapeado para Til/Crase
      case 0x4A: Keyboard.write('/'); break;  // Mapeado para Ponto e Vírgula
      case 0x51: Keyboard.write(0xDC); break; // Mapeado para Barra/Interrogação
      case 0x4E: Keyboard.write('-'); break; 
      case 0x55: Keyboard.write('='); break;
      case 0x41: Keyboard.write(','); break; 
      case 0x49: Keyboard.write('.'); break;
    }
  }
}
