/*Aluno: Daniel Lucas Silva do Nascimento

/* Necessário para funções e macros básicas */
#include <avr/io.h>

/* Os próximos dois includes são necessários quando se usa interrupçoes */
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#define LED_PORT   PORTB
#define LED_DDR    DDRB
#define LED_PINNBR DDB1


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
volatile uint8_t passo = 1;

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


/* A função main() implementa uma onda digital quadrada com o ciclo de
   trabalho variando de acordo com uma função triangular.  Use o valor
   da onda para acender ou apagar o led que está no 6º pino da porta
   B. */
int main(void) {
    uint8_t ativo;
    uint8_t i;

    /* Faça a configuração do que for necessário aqui */
    DDRD  &= ~(1 << PD2); //seta o bit 2 da porta D como entrada
    PORTD |=  (1 << PD2); //entrada em nível logico alto para resistor de pullup

    /*
     * Configure os registradores EICRA, EIFR e EIMSK aqui para habilitar
     * (ou não) a interrupção associada à mudança no pino INT0 (veja Seção
     * 13.2 da Folha de Dados do ATMega328p).
     */
    EIMSK |= (1 << INT0);
    EICRA = (1 << ISC00); //mudança logica

    /* habilita interrupções globalmente */
    sei(); 

    /* Configuração básica do led da placa iniciando com o led apagado */
    LED_DDR  |=  (1 << LED_PINNBR); //seta como saida
    LED_PORT &= ~(1 << LED_PINNBR); //desliga o led
/*
    /* Implementação de uma onda quadrada com ciclo de trabalho
       variável. O ciclo de trabalho varia a cada QTDE_PULSOS pulsos
       da onda quadrada */
    while (1) {
        ativo = onda_triangular();
        for (i=0; i<QTDE_PULSOS; i++) {
            LED_PORT |= (1 << LED_PINNBR);
            delay_dezenas_us(ativo);
	 LED_PORT &= ~(1 << LED_PINNBR);
            delay_dezenas_us(PERIODO - ativo);
        }
    }

    return 0;
}

ISR(INT0_vect) {
       
        if (passo == 1)
	passo=3;
        else 
	passo = 1;
}
