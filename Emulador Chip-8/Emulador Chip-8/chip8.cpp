#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>

void chip8::resetar()
{
	codigo_op = 0;
	i = 0;
	flag_Tela = 0;
	PC = 0x200; //O CHIP-8 ocupava os primeiros 512 bytes de memória, por isso começamos na posição 0x200
	posicao_atual = 0;

	//Limpa os registradores, posição e teclas
	for (int i = 0; i < 16; i++)
		registrador[i] = 0;
	posicao[i] = 0;
	tecla[i] = 0;

	//Lima toda a memória
	for (int i = 0; i < 4096; i++)
		memoria[i] = 0;

	timer_delay = 0;
	timer_som = 0;
	flag_Tela = true; //Limpa a tela
}

chip8::chip8()
{
}

void chip8::emula_Ciclo()
{
	//Copia o código op da memória
	codigo_op = (memoria[PC] << 8) || memoria[PC + 1]; //adiciona 8 zeros e usa a operação lógica OR para juntar a instrução que está dividida em memoria[PC] e memoria [PC+1]

													   //Switch que verifica as operações
	switch (codigo_op & 0xF000) { //Utiliza a operação lógica AND para fazer uma máscara de bits que irá verificar apenas os 4 bits da esquerda para direita (os bits que contem a operação)
								  //Casos os 4 primeiros bits da esquerda para a direita derem 0 sabemos que existem duas instruções possíveis
	case (0x0000):
		switch (codigo_op & 0x000F)//máscara de bits para verificar os 4 últimos e determinar a instrução
		{
		case (0x0000): // 0x00E0: Limpa a tela
			for (int i = 0; i < 2048; ++i)
				graficos[i] = 0x0;
			flag_Tela = true;
			PC += 2;
			break;

		case (0x000E): // 0x00EE: Retorna da subrotina

			break;

		default:
			printf("Código OP desconhecido[0x0000]: 0x%X\n", codigo_op);
		}

	case (0x1000):// 0x1NNN: Pula para o endereço NNN
		PC = codigo_op & 0x0FFF; //Máscara de bits para pegar o endereço
		break;

	case (0x2000):// 0x2NNN: Chama a subrotina no endereço NNN
		posicao[posicao_atual] = PC; //Armazena a posição atual do código para podermos retornar depois
		PC = codigo_op & 0x0FFF; //Máscara de bits para pegar o endereço
		posicao_atual++;
		break;

	case (0x3000):// 0x3XNN: Pula a instrução se o Registrador [X] for igual a NN
		if (registrador[(codigo_op & 0x0F00) >> 8] == (codigo_op & 0x00FF)) {//Pega o número do registrador e valor a ser comparado com o conteúdo do registrador
			PC = PC + 4; // Caso o valores sejam iguais, incrementa o PC contando a instrução 0x3XNN e a instrução pulada
		}
		else {
			PC = PC + 2; // Do contrário conta apenas a instrução 0x3XNN
		}
		break;

	case (0x4000):// 0x4XNN: Pula a instrução se o Registrador [X] for diferente a NN
		if (registrador[(codigo_op & 0x0F00) >> 8] != (codigo_op & 0x00FF)) {//Pega o número do registrador e valor a ser comparado com o conteúdo do registrador
			PC = PC + 4; // Caso o valores sejam diferentes, incrementa o PC contando a instrução 0x3XNN e a instrução pulada
		}
		else {
			PC = PC + 2; // Do contrário conta apenas a instrução 0x4XNN
		}
		break;

	case (0x5000):// 0x5XY0: Pula a instrução se o Registrador [X] for diferente do Registrador [Y]
		if (registrador[(codigo_op & 0x0F00) >> 8] != registrador[(codigo_op & 0x00F0) >> 4]) {//Pega os número dos registradores X e Y e compara
			PC = PC + 4; // Caso o valores sejam iguais, incrementa o PC contando a instrução 0x5XY0 e a instrução pulada
		}
		else {
			PC = PC + 2; // Do contrário conta apenas a instrução 0x5XY0
		}
		break;

	case (0x6000):// 0x6XNN: Seta o Registrador [X] for igual a NN
		registrador[(codigo_op & 0x0F00) >> 8] = (codigo_op & 0x00FF);//Pega o valor em NN e seta no registrador [X]
		PC = PC + 2; //Conta a instrução 0x6XNN
		break;

	case (0x7000):// 0x7XNN: Soma NN No Registrador [X]
		registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] + (codigo_op & 0x00FF);//Pega o valor em NN e soma no registrador [X]
		PC = PC + 2; //Conta a instrução 0x7XNN
		break;

	case (0x8000):
		switch (codigo_op & 0x000F)
		{
		case (0x0000):// 0x8XY0 Seta o Registrador [X] igual ao valor do Registrador [Y]
			registrador[(codigo_op & 0x0F00) >> 8] == registrador[(codigo_op & 0x00F0) >> 4];
				PC = PC + 2; //Conta a instrução 0x8XY0
			break;

		case (0x0001):// 0x8XY1 Realiza a operação lógica OR entre os registradores [X] e [Y]
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] | registrador[(codigo_op & 0x00F0) >> 4];
			PC = PC + 2; //Conta a instrução 0x8XY1
			break;

		case (0x0002):// 0x8XY2 Realiza a operação lógica AND entre os registradores [X] e [Y]
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] & registrador[(codigo_op & 0x00F0) >> 4];
			PC = PC + 2; //Conta a instrução 0x8XY2
			break;

		case (0x0003):// 0x8XY3 Realiza a operação lógica XOR entre os registradores [X] e [Y]
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] ^ registrador[(codigo_op & 0x00F0) >> 4];
			PC = PC + 2; //Conta a instrução 0x8XY3
			break;

		case (0x0004):// 0x8XY4 Soma os registradores [X] e [Y] e guarda em [X]
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] + registrador[(codigo_op & 0x00F0) >> 4];
			if (flag_Tela) { //Seta o registrador [F] para 1 se tiver carry
				registrador[15] = 1;
			}
			else {//Do contrário seta para 0
				registrador[15] = 0;
			}
			PC = PC + 2; //Conta a instrução 0x8XY4
			break;

		case (0x0005):// 0x8XY5 Subtrai os registradores [Y] de [X] e guarda em [X]. Registrador [15] = 1 se tiver borrow.
			if (registrador[(codigo_op & 0x0F00)] > registrador[(codigo_op & 0x00F0) >> 4]) { //Verifica o conteúdo dos registradores [X] e [Y]
				registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] - registrador[(codigo_op & 0x00F0) >> 4];
				registrador[15] = 0; //Sem borrow
			}
			else {
				registrador[15] = 1; //Tem borrow
			}
			PC = PC + 2; //Conta a instrução 0x8XY5
			break;

		case (0x0006):// 0x8XY6 Seta o Registrador [15] com o último bit do Registrador [X] e joga o Registrador [X]  1 bit à direita
			registrador[15] = registrador[(codigo_op & 0x0F00) >> 8] & 0x01; //Máscara de bits para pegar o bit LSB
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] >> 1;
			PC = PC + 2; //Conta a instrução 0x8XY6

		case (0x0007):// 0x8XY7 Subtrai os Registradores [X] de [Y] e guarda em [X]. Registrador [15] = 1 se tiver borrow.
			if (registrador[(codigo_op & 0x0F00)] > registrador[(codigo_op & 0x00F0) >> 4]) { //Verifica o conteúdo dos registradores [X] e [Y]
				registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 4] - registrador[(codigo_op & 0x00F0) >> 8];
				registrador[15] = 0; //Sem borrow
			}
			else {
				registrador[15] = 1; //Tem borrow
			}
			PC = PC + 2; //Conta a instrução 0x8XY7
			break;

		case (0x000E):// 0x8XYE Seta o Registrador [15] com o último bit do Registrador [X] e joga o Registrador [X]  1 bit à direita
			registrador[15] = registrador[(codigo_op & 0x0F00) >> 8] >> 7; //Máscara de bits para pegar o bit MSB
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] << 1;
			PC = PC + 2; //Conta a instrução 0x8XYE

		default:
			printf("Código OP desconhecido [0x8000]: 0x%X\n", codigo_op);
		}
		break;

	case (0x9000): // 0x9XY0: Pula a próxima instrução se o Registrador [X]for diferente do Registrador [Y]
		if (registrador[(codigo_op & 0x0F00) >> 8] != registrador[(codigo_op & 0x00F0) >> 4])
			PC = PC + 4;
		else
			PC = PC + 2;
		break;

	case (0xA000): // 0xANNN: Seta I igual ao endereço NNN
		i = (codigo_op & 0x0FFF);
		PC = PC + 2;
		break;

	case (0xB000): // 0xB000: Pula para o endereço de NNN + registrador [0]
		PC = registrador[0] + 0x0FFF;
		break;

	case (0xC000): // 0xCXNN: Seta o registrador [X] com o resultado da operação entre NN e um número aleatório (0 à 255)
		registrador[(codigo_op & 0x0F00) >> 8] = (codigo_op & 0x00FF) & (rand() % 0xFF);
		PC = PC + 2;
		break;

	case (0xD000): // 0xDXYN: Desenha um sprite 8XN nas coordenadas (Registrado[X], Registrador[Y])
				   // Each row of 8 pixels is read as bit-coded starting from memory location I;
				   // I value doesn't change after the execution of this instruction.
				   // VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
				   // and to 0 if that doesn't happen
	{
		unsigned short x = registrador[(codigo_op & 0x0F00) >> 8];
		unsigned short y = registrador[(codigo_op & 0x00F0) >> 4];
		unsigned short altura = codigo_op & 0x000F;
		unsigned short pixel;

		registrador[0xF] = 0;
		for (int linha_y = 0; linha_y < altura; linha_y++)
		{
			pixel = memoria[i + linha_y];
			for (int linha_x = 0; linha_x < 8; linha_x++)
			{
				if ((pixel & (0x80 >> linha_x)) != 0)
				{
					if (graficos[(x + linha_x + ((y + linha_y) * 64))] == 1)
					{
						registrador[0xF] = 1;
					}
					graficos[x + linha_x + ((y + linha_y) * 64)] ^= 1;
				}
			}
		}

		flag_Tela = true;
		PC = PC + 2;
	}
	break;
	case (0xF000):

		switch (codigo_op & 0x00FF)
		{
		case (0x0007): // 0xFX07: Pega o valor do timer de delay e joga no Registrador[X]
			registrador[(codigo_op & 0x0F00) >> 8] = timer_delay;
			PC = PC + 2;
			break;

		case (0x000A): // 0xFX0A: Pega o valor do timer de delay e joga no Registrador[X]
			//registrador[(codigo_op & 0x0F00) >> 8] = teclas;
			PC = PC + 2;
			break;

		case (0x0015): // 0xFX15: Pega o valor do Registrador[X] e joga no timer de delay
			timer_delay = registrador[(codigo_op & 0x0F00) >> 8];
			PC = PC + 2;
			break;

		case (0x0017): // 0xFX18: Pega o valor do Registrador[X] e joga no timer de som
			timer_som = registrador[(codigo_op & 0x0F00) >> 8];
			PC = PC + 2;
			break;

		case (0x001E): // 0xFX18: Soma o Registrador[X] + I e joga o resultado em I
			i = i + registrador[(codigo_op & 0x0F00) >> 8];
			PC = PC + 2;
			break;

		case (0x0029): // 0xFX29: Pega o valor do timer de delay e joga no Registrador[X]
			i = i + registrador[(codigo_op & 0x0F00) >> 8];
			PC = PC + 2;
			break;

		case (0x0033): // 0xFX33: Guarda a representação binária do valor no Registrador[X]
			memoria[i] = registrador[(codigo_op & 0x0F00) >> 8] / 100; //3 dígitos mais significativos
			memoria[i + 1] = (registrador[(codigo_op & 0x0F00) >> 8] / 10) % 10; //dígito do meio
			memoria[i + 2] = (registrador[(codigo_op & 0x0F00) >> 8] % 100) % 10; //Dígito menos significativo
			PC = PC + 2;
			break;

		case (0x0055): // 0xFX55: Guarda os dados do Registrador [0] ao Registrador[X] na memória
			for (int contador = 0; contador <= registrador[(codigo_op & 0x0F00) >> 8]; contador++) {
				memoria[i] = registrador[contador];
				i = i + 1;
			}
			PC = PC + 2;
			break;

		case (0x0065): // 0xFX65: Pega os dados da memória e guarda do Registrador [0] ao Registrador[X]
			for (int contador = 0; contador <= registrador[(codigo_op & 0x0F00) >> 8]; contador++) {
				registrador[contador] = memoria[i];
				i = i + 1;
			}
			PC = PC + 2;
			break;

		default:
			printf("Código OP desconhecido [0x8000]: 0x%X\n", codigo_op);
		}
	default:
		printf("Código OP desconhecido [0x8000]: 0x%X\n", codigo_op);
	}
}
