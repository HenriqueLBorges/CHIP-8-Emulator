#ifndef CHIP8_H_INCLUDED
#define CHIP8_H_INCLUDED
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#endif // CHIP8_H_INCLUDED
class chip8 {
public:
	chip8();
	void emula_Ciclo();//Emula o ciclo do CHIP-8
	bool carregarJogo(const char * filename);//Carrega a ROM na memória do CHIP-8
	void ALLEGRO_EVENT_KEY_DOWN(); //Verifica a tecla pressionada
	void ALLEGRO_EVENT_KEY_UP(); //Zera o teclado
	void resetar(); //Reseta o CHIP-8
	bool flag_Tela; //Status atual do registrador [15] (Carry Flag)
	bool inicializar(); //Testa as configurações antes de iniciar o emulador
	ALLEGRO_FONT *fonte = NULL;
	unsigned char tecla[16]; //As teclas do Chip-8 vão de 0 à F
	

private:
	ALLEGRO_SAMPLE *sample = NULL; //Variável que será tocado quando o timer ativar
	ALLEGRO_DISPLAY *janela; //Janela do emulador

	unsigned short codigo_op; //Código da operação
	unsigned char registrador[16]; //15 registradores (V0...VE) de 8 bits do CHIP-8 - 16º = 'carry flag'
	unsigned short i; //Index
	unsigned short PC; // Program Counter do CHIP-8

	unsigned short posicao[16]; //Em caso de jumps ou calls saberemos para onde voltar no programa
	unsigned short posicao_atual; //Guarda a posição atual no vetor de posições

	unsigned char memoria[4096]; //Memória total = 4K

	unsigned char timer_delay;
	unsigned char timer_som; //ativa o som quando o timer é diferente de 0

};



