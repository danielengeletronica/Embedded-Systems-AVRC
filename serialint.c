/* Necessário para funções e macros básicas 
Aluno: Daniel Lucas Silva do Nascimento
*/
#include <avr/io.h>

#include <stdint.h>
/* Os próximos dois includes são necessários quando se usa interrupçoes */
#include <avr/cpufunc.h>
#include <avr/interrupt.h>

#define LED_PORT   PORTB
#define LED_DDR    DDRB
#define LED_PINNBR DDB1
#define USART_BAUDRATE 500000
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

/* -------------------------------------*/
/*
 * Interrupt handlers devem ser definidos com a macro
 * ISR(), que é definida no arquivo avr/interrupt.h.
 * Veja também a página
 * http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
 * 
 * Nesse caso, toda a funcionalidade necessária é bem simples e portanto
 * fazemos tudo na própria interrupção.
 */
 /* Função implementando a conmversão de um byte para dois caracteres
   ASCII representando o valor hexadecimal do byte */
void hexa(uint8_t a, char res[2]) {
    res[0] = (a & 0x0F) + ((a & 0x0F) >= 10 ? 'A' - 10 : '0');
    a >>= 4;
    res[1] = (a & 0x0F) + ((a & 0x0F) >= 10 ? 'A' - 10 : '0');
}

/* Variável global representando quão rápido a onda triangular cresce
   (ou decresce).  Observe o uso de volatile abaixo.  Caso não
   entenda, pergunte ao professor */
volatile uint8_t passo = 1;

/* Implementação de uma onda triangular variando de 0 a PERIODO */

/* Consante definindo período da onda quadrada relacionada à
   inensidade do led.  Não pode ser maior do que 255 */
#define PERIODO 100
/* Número de pulsos da onda quadrada com o ciclo de trabalho fixo */
#define QTDE_PULSOS 10

/* Função implementado delay múltiplo de 10 microsegundos */
void delay_dezenas_us(uint16_t dezenas) {
    int16_t i;
    uint16_t j;

    for (j=0; j<dezenas; j++)
        for (i=32; i>=0; i--)
            asm("nop");
}

/* Variável global representando quão rápido a onda triangular cresce
   (ou decresce).  Observe o uso de volatile abaixo.  Caso não
   entenda, pergunte ao professor */
/* Implementação de uma onda triangular variando de 0 a PERIODO */
uint8_t onda_triangular(void) {
    static int8_t valor_atual = 0;
    static uint8_t crescente = 1;

    if (crescente)
        valor_atual += passo;
    else
        valor_atual -= passo;

    if (valor_atual > PERIODO) {
        valor_atual = PERIODO;
        crescente = 0;
    } else if (valor_atual < 0) {
        valor_atual = 0;
        crescente = 1;
    }

    return valor_atual;
}

/* Implementação das funções de verificação de flags */
uint8_t int0_ativo() {

    return (EIFR & (1 << INTF0)) == 0;
}

uint8_t USART_RXC_ativo() {
	
	//enable transmission and reception
	UCSR0B |= (1<<RXEN0);
	return  ((UCSR0B & (1<<RXEN0)) == 1);
}

uint8_t USART_UDRE_ativo() {
    UCSR0B |= (1<<TXEN0);
	return ((UCSR0B & (1<<TXEN0)) == 1);
}


/* Implemente aqui as funções usart0_getc() e usart0_putc() com as
   assinaturas mostradas */
/* A função usart0_getc() deve retornar -1 se não houver nada para ser
   lido.  Caso contrário, deve retornar o dado no registrador DR */
int16_t usart0_getc(void) {
    /* Seu código aqui */
	// Wait for byte to be received
  if((UCSR0A&(1<<RXC0)))
    	return UDR0;
   else 
	return -1;
}

/* A função usart0_putc() deve retornar -1 se o registrador DR não
   estiver vazio.  Caso contrário, deve retornar 0 e escrever o valor
   do parâmetro c no registrador */
int16_t usart0_putc(char c) {
    /* Seu código aqui */
	//wait while previous byte is completed
	while(!(UCSR0A&(1<<UDRE0))){};
	// Transmit data
	UDR0 = c;
	return 0;
}

/* A função main() implementa uma onda digital quadrada com o ciclo de
   trabalho variando de acordo com uma função triangular.  Use o valor
   da onda para acender ou apagar o led que está no 6º pino da porta
   B. */
char aux[2]; 
int c;
ISR(INT0_vect) {
c++;
hexa(c, aux);
usart0_putc(aux[1]);
usart0_putc(aux[0]);
}

/* -------------------------------------*/
int main(void) {
    /* Configura o pino INT0 (mapeado no pino PD2 da porta D) como
     * entrada com pull-up (ver seção 14.2.1 da folha de dados do
     * ATMega328p) */
 uint8_t ativo;
    uint8_t i;

    DDRD  &= ~(1 << PD2); //seta o bit 2 da porta D como entrada
    PORTD |=  (1 << PD2); //entrada em nível logico alto para resistor de pullup
    int8_t passo_aux;
    /*
     * Configure os registradores EICRA, EIFR e EIMSK aqui para habilitar
     * (ou não) a interrupção associada à mudança no pino INT0 (veja Seção
     * 13.2 da Folha de Dados do ATMega328p).
     */
    EIMSK |= (1 << INT0);
    EICRA = (1 << ISC00)|(1 << ISC01); //borda de subida
    
    /* habilita interrupções globalmente */
    sei(); 
	c = 0;
	
	// Set baud rate
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	// Set frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	USART_RXC_ativo() ;
	USART_UDRE_ativo() ;
	
	LED_DDR  |=  (1 << LED_PINNBR); //seta como saida
    	LED_PORT &= ~(1 << LED_PINNBR); //desliga o led

    /* Loop infinito */
    while (1) {
        ativo = onda_triangular();
        for (i=0; i<QTDE_PULSOS; i++) {
            LED_PORT |= (1 << LED_PINNBR);
            delay_dezenas_us(ativo);
	 LED_PORT &= ~(1 << LED_PINNBR);
            delay_dezenas_us(PERIODO - ativo);
	passo_aux = usart0_getc();
	if (passo_aux >= 1 && passo_aux<11)
		passo = passo_aux;
	}
	
        	
    }

    return 0;
}
