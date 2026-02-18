Projeto Teclado PS2 Marcelo - Finalizado 16-02-2026

# Projeto Teclado PS/2 Customizado - Marcelo

## ? Resumo do Dia: 17/02/2026

Hoje o foco foi a estabilização do firmware e a resolução de conflitos de hardware entre o Arduino Pro Micro e o Raspberry Pi 4.

### ? Conquistas e Versão Atual
- **Versão Estável: 1.6** (Restaurada para garantir a base de código que funciona).
- **Fix da Barra Invertida:** Implementação do bypass bruto (Usage ID 100) para suporte ao ABNT2 no Raspberry Pi.
- **Setas de Navegação:** Recuperadas e testadas com sucesso.

### ? Desafios Identificados (Capítulos para o Ebook)
1. **O Mistério da Regravação:** O teclado só é reconhecido pelo Raspberry Pi após o upload do firmware. O Reset físico e o Hot-plug não foram suficientes. Suspeita-se de necessidade de atraso no Power-on ou Reset de hardware mais agressivo.
2. **O Zero Fantasma (0):** Identificada a presença de caracteres indesejados durante a digitação. 
   - *Ações realizadas:* Ressoldagem dos cabos.
   - *Próximo passo:* Instalação de capacitor de filtro (10nF/100nF) no pino de Clock (IRQ) para eliminar ruído eletromagnético.

### ?? Hardware Utilizado
- Arduino Pro Micro (ATmega32U4).
- Raspberry Pi 4.
- Teclado PS/2 antigo (Engenharia Reversa).
