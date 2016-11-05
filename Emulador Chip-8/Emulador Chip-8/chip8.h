#ifndef CHIP8_H_INCLUDED
#define CHIP8_H_INCLUDED



#endif // CHIP8_H_INCLUDED
class chip8 {
public:
	chip8();
	void emula_Ciclo(); //Emula um ciclo do CHIP-8
	void resetar(); //Reseta o CHIP-8
	bool flag_Tela; //Status atual do registrador [15] (Carry Flag)

private:

	unsigned short codigo_op; //C�digo da opera��o
	unsigned char registrador[16]; //15 registradores (V0...VE) de 8 bits do CHIP-8 - 16� = 'carry flag'
	unsigned short i; //Index
	unsigned short PC; // Program Counter do CHIP-8

	unsigned short posicao[16]; //Em caso de jumps ou calls saberemos para onde voltar no programa
	unsigned short posicao_atual; //Guarda a posi��o atual no vetor de posi��es

	unsigned char memoria[4096]; //Mem�ria total = 4K

	unsigned char graficos[64 * 32]; //O Chip-8 tem um display de 64x32 pixels
	unsigned char tecla[16]; //As teclas do Chip-8 v�o de 0 � F

	unsigned char timer_delay;
	unsigned char timer_som; //ativa o som quando o timer � diferente de 0

};



