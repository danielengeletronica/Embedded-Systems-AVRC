/* Necessário para funções e macros básicas 
Aluno: Daniel Lucas Silva do Nascimento
*/
#define 	F_CPU   1000000UL
#include <avr/io.h>
#include <util/delay.h>

/* Os próximos dois includes são necessários quando se usa interrupçoes */
#include <avr/cpufunc.h>
#include <avr/interrupt.h>

#define LED_PORT   PORTB
#define LED_DDR    DDRB
#define LED_PINNBR DDB1

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
ISR(INT0_vect) {
      
       LED_PORT |= (1 << LED_PINNBR);
       _delay_ms (5000);
}

/* -------------------------------------*/
int main(void) {
    /* Configura o pino INT0 (mapeado no pino PD2 da porta D) como
     * entrada com pull-up (ver seção 14.2.1 da folha de dados do
     * ATMega328p) */
    DDRD  &= ~(1 << PD2); //seta o bit 2 da porta D como entrada
    PORTD |=  (1 << PD2); //entrada em nível logico alto para resistor de pullup

    /*
     * Configure os registradores EICRA, EIFR e EIMSK aqui para habilitar
     * (ou não) a interrupção associada à mudança no pino INT0 (veja Seção
     * 13.2 da Folha de Dados do ATMega328p).
     */
    EIMSK |= (1 << INT0);
    EICRA = (1 << ISC00)|(1 << ISC01); //borda de descida do INT0 interrupt


    /* habilita interrupções globalmente */
    sei(); 

    /* Configuração básica do led da placa iniciando com o led apagado */
    LED_DDR  |=  (1 << LED_PINNBR); //seta como saida
    LED_PORT &= ~(1 << LED_PINNBR); //desliga o led
/*

*/
    /* Loop infinito */
    while (1) {
	
        if (LED_PORT & (1 << LED_PINNBR))	
    	 {  LED_PORT &= ~(1 << LED_PINNBR);}
    }

    return 0;
}
