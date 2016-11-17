#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

/*bool chip8::inicializar()
{
	if (!al_init()) //Testa a inicializa��o do Allegro
	{
		fprintf(stderr, "Falha ao inicializar Allegro.\n");
		return false;
	}

	if (!al_install_audio()) //Testa a incializa��o do �udio 
	{
		fprintf(stderr, "Falha ao inicializar �udio.\n");
		return false;
	}

	if (!al_init_acodec_addon()) //Testa os codecs de �udio
	{
		fprintf(stderr, "Falha ao inicializar codecs de �udio.\n");
		return false;
	}

	if (!al_reserve_samples(1)) //Testa os canais
	{
		fprintf(stderr, "Falha ao alocar canais de �udio.\n");
		return false;
	}

	if (!al_install_keyboard()) //Testa o teclado
	{
		fprintf(stderr, "Falha ao inicializar teclado.\n");
		return false;
	}

	janela = al_create_display(64, 32); //O Chip-8 tem um display de 64x32 pixels

	if (!janela) //Testa a janela
	{
		fprintf(stderr, "Falha ao criar a janela.\n");
		return false;
	}

	sample = al_load_sample("Som.wav"); //Seta o som do Timer;
	if (!sample) //Testa o som
	{
		fprintf(stderr, "Falha ao carregar sample.\n");
		al_destroy_display(janela);
		return false;
	}
	if (!al_init_primitives_addon())
	{
		fprintf(stderr, "Falha ao inicializar add-on de primitivas.\n");
		return false;
	}
	al_init_font_addon();

	if (!al_init_ttf_addon())
	{
		fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
		return false;
	}

	fonte = al_load_font("arial.ttf", 72, 0);
	if (!fonte)
	{
		fprintf(stderr, "Falha ao carregar \"fonte arial.ttf\".\n");
		al_destroy_display(janela);
		return false;
	}

	fila_eventos = al_create_event_queue();
	if (!fila_eventos)
	{
		fprintf(stderr, "Falha ao criar fila de eventos.\n");
		al_destroy_display(janela);
		return false;
	}
	al_register_event_source(fila_eventos, al_get_keyboard_event_source()); //Prepara a fila de evenos para pegar qualquer evento relacionado ao teclado
	al_register_event_source(fila_eventos, al_get_display_event_source(janela)); //Prepara a fila de evenos para pegar qualquer evento relacionado � janela
	al_set_window_title(janela, "Emulador Chip-8"); //Coloca o t�tulo da janela
    white = al_map_rgb(0, 0, 0);
	black = al_map_rgb(255, 255, 255);
	return true;
}*/
chip8::chip8()
{
}
unsigned char chip8_fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void chip8::resetar()
{
	codigo_op = 0;
	i = 0;
	flag_Tela = 0;
	PC = 0x200; //O CHIP-8 ocupava os primeiros 512 bytes de mem�ria, por isso come�amos na posi��o 0x200
	posicao_atual = 0;

	//Limpa o display
	for (int i = 0; i < 2048; ++i)
		gfx[i] = 0;

	//Limpa os registradores, posi��o e teclas
	for (int i = 0; i < 16; i++) {
		registrador[i] = 0;
		posicao[i] = 0;
		tecla[i] = 0;
	}

	//Limpa toda a mem�ria
	for (int i = 0; i < 4096; i++)
		memoria[i] = 0;

	timer_delay = 0;
	timer_som = 0;
	flag_Tela = true; //Limpa a tela

	// Carrega o fontset
	for (int i = 0; i < 80; ++i)
		memoria[i] = chip8_fontset[i];
}

void chip8::emula_Ciclo()
{
	//Copia o c�digo op da mem�ria
	codigo_op = memoria[PC] << 8 | memoria[PC + 1]; //adiciona 8 zeros e usa a opera��o l�gica OR para juntar a instru��o que est� dividida em memoria[PC] e memoria [PC+1]
	printf("Executando OP 0x%04X da memoria em [%04X], I= %02X e PC =:%02X\n", codigo_op, memoria[PC], i, PC);
	//Switch que verifica as opera��es
	switch (codigo_op & 0xF000) { //Utiliza a opera��o l�gica AND para fazer uma m�scara de bits que ir� verificar apenas os 4 bits da esquerda para direita (os bits que contem a opera��o)
								  //Casos os 4 primeiros bits da esquerda para a direita derem 0 sabemos que existem duas instru��es poss�veis
		case 0x0000:
			switch (codigo_op & 0x000F)//m�scara de bits para verificar os 4 �ltimos e determinar a instru��o
			{
				case 0x0000: //00E0: Limpa a tela
					al_clear_to_color(al_map_rgb(0, 0, 0));
					for (int i = 0; i < 2048; ++i)
						gfx[i] = 0;
					flag_Tela = true;
					PC += 2;
				break;

				case 0x000E: //0x00EE: Retorna da subrotina
					posicao_atual = posicao_atual -1;			// 16 Niveis da pilha, o ponteiro da pilha � decrementado pra previnir erro
					PC = posicao[posicao_atual];	// Coloca o endere�o de retorno da pilha de volta no PC					
					PC = PC + 2;		
				break;

				default:
					printf("Codigo OP desconhecido[0x0000]: 0x%X\n", codigo_op);
					printf("Entrou no Switch das OPs 00E0 e 0x00EE\n");
			}
		break;

		case 0x1000:// 0x1NNN: Pula para o endere�o NNN
			PC = codigo_op & 0x0FFF; //M�scara de bits para pegar o endere�o
		break;

		case 0x2000:// 0x2NNN: Chama a subrotina no endere�o NNN
			posicao[posicao_atual] = PC; //Armazena a posi��o atual do c�digo para podermos retornar depois
			PC = codigo_op & 0x0FFF; //M�scara de bits para pegar o endere�o
			posicao_atual++;
		break;

		case 0x3000:// 0x3XNN: Pula a instru��o se o Registrador [X] for igual a NN
			if (registrador[(codigo_op & 0x0F00) >> 8] == (codigo_op & 0x00FF)) {//Pega o n�mero do registrador e valor a ser comparado com o conte�do do registrador
				PC = PC + 4; // Caso o valores sejam iguais, incrementa o PC contando a instru��o 0x3XNN e a instru��o pulada
			}
			else {
				PC = PC + 2; // Do contr�rio conta apenas a instru��o 0x3XNN
			}
			break;

		case 0x4000:// 0x4XNN: Pula a instru��o se o Registrador [X] for diferente a NN
			if (registrador[(codigo_op & 0x0F00) >> 8] != (codigo_op & 0x00FF)) {//Pega o n�mero do registrador e valor a ser comparado com o conte�do do registrador
				PC = PC + 4; // Caso o valores sejam diferentes, incrementa o PC contando a instru��o 0x3XNN e a instru��o pulada
			}
			else {
				PC = PC + 2; // Do contr�rio conta apenas a instru��o 0x4XNN
			}
		break;

		case 0x5000:// 0x5XY0: Pula a instru��o se o Registrador [X] for diferente do Registrador [Y]
			if (registrador[(codigo_op & 0x0F00) >> 8] != registrador[(codigo_op & 0x00F0) >> 4]) {//Pega os n�mero dos registradores X e Y e compara
				PC = PC + 4; // Caso o valores sejam iguais, incrementa o PC contando a instru��o 0x5XY0 e a instru��o pulada
			}
			else {
				PC = PC + 2; // Do contr�rio conta apenas a instru��o 0x5XY0
			}
			break;

		case 0x6000:// 0x6XNN: Seta o Registrador [X] for igual a NN
			registrador[(codigo_op & 0x0F00) >> 8] = (codigo_op & 0x00FF);//Pega o valor em NN e seta no registrador [X]
			PC = PC + 2; //Conta a instru��o 0x6XNN
		break;

		case 0x7000:// 0x7XNN: Soma NN No Registrador [X]
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] + (codigo_op & 0x00FF);//Pega o valor em NN e soma no registrador [X]
			PC = PC + 2; //Conta a instru��o 0x7XNN
		break;

		case 0x8000:
			switch (codigo_op & 0x000F)
			{
				case 0x0000:// 0x8XY0 Seta o Registrador [X] igual ao valor do Registrador [Y]
					registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x00F0) >> 4];
					PC = PC + 2; //Conta a instru��o 0x8XY0
				break;

				case 0x0001:// 0x8XY1 Realiza a opera��o l�gica OR entre os registradores [X] e [Y]
					registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] | registrador[(codigo_op & 0x00F0) >> 4];
					PC = PC + 2; //Conta a instru��o 0x8XY1
				break;

				case 0x0002:// 0x8XY2 Realiza a opera��o l�gica AND entre os registradores [X] e [Y]
					registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] & registrador[(codigo_op & 0x00F0) >> 4];
					PC = PC + 2; //Conta a instru��o 0x8XY2
				break;

				case 0x0003:// 0x8XY3 Realiza a opera��o l�gica XOR entre os registradores [X] e [Y]
					registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] ^ registrador[(codigo_op & 0x00F0) >> 4];
					PC = PC + 2; //Conta a instru��o 0x8XY3
				break;

				case 0x0004:// 0x8XY4 Soma os registradores [X] e [Y] e guarda em [X]
					registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] + registrador[(codigo_op & 0x00F0) >> 4];
					if (flag_Tela) { //Seta o registrador [F] para 1 se tiver carry
						registrador[15] = 1;
					}
					else {//Do contr�rio seta para 0
						registrador[15] = 0;
					}
					PC = PC + 2; //Conta a instru��o 0x8XY4
				break;

				case 0x0005:// 0x8XY5 Subtrai os registradores [Y] de [X] e guarda em [X]. Registrador [15] = 1 se tiver borrow.
					if (registrador[(codigo_op & 0x0F00)] > registrador[(codigo_op & 0x00F0) >> 4]) { //Verifica o conte�do dos registradores [X] e [Y]
						registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] - registrador[(codigo_op & 0x00F0) >> 4];
						registrador[15] = 0; //Sem borrow
					}
					else {
						registrador[15] = 1; //Tem borrow
					}
					PC = PC + 2; //Conta a instru��o 0x8XY5
				break;

				case 0x0006:// 0x8XY6 Seta o Registrador [15] com o �ltimo bit do Registrador [X] e joga o Registrador [X]  1 bit � direita
					registrador[15] = registrador[(codigo_op & 0x0F00) >> 8] & 0x01; //M�scara de bits para pegar o bit LSB
					registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] >> 1;
					PC = PC + 2; //Conta a instru��o 0x8XY6
				break;

				case 0x0007:// 0x8XY7 Subtrai os Registradores [X] de [Y] e guarda em [X]. Registrador [15] = 1 se tiver borrow.
					if (registrador[(codigo_op & 0x0F00)] > registrador[(codigo_op & 0x00F0) >> 4]) { //Verifica o conte�do dos registradores [X] e [Y]
						registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 4] - registrador[(codigo_op & 0x00F0) >> 8];
						registrador[15] = 0; //Sem borrow
					}
					else {
						registrador[15] = 1; //Tem borrow
					}
					PC = PC + 2; //Conta a instru��o 0x8XY7
				break;

				case 0x000E:// 0x8XYE Seta o Registrador [15] com o �ltimo bit do Registrador [X] e joga o Registrador [X]  1 bit � direita
					registrador[15] = registrador[(codigo_op & 0x0F00) >> 8] >> 7; //M�scara de bits para pegar o bit MSB
					registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] << 1;
					PC = PC + 2; //Conta a instru��o 0x8XYE
				break;

				default:
					printf("Codigo OP desconhecido [0x8000]: 0x%X\n", codigo_op);
					printf("Entrou no Switch das OPs 0x8000\n");
				}
		break;
	
		case 0x9000: // 0x9XY0: Pula a pr�xima instru��o se o Registrador [X]for diferente do Registrador [Y]
			if (registrador[(codigo_op & 0x0F00) >> 8] != registrador[(codigo_op & 0x00F0) >> 4])
				PC = PC + 4;
			else
				PC = PC + 2;
			break;

		case 0xA000: // 0xANNN: Seta I igual ao endere�o NNN
			i = (codigo_op & 0x0FFF);
			PC = PC + 2;
		break;

		case 0xB000: // 0xB000: Pula para o endere�o de NNN + registrador [0]
			PC = registrador[0] + 0x0FFF;
		break;

		case 0xC000: // 0xCXNN: Seta o registrador [X] com o resultado da opera��o entre NN e um n�mero aleat�rio (0 � 255)
			registrador[(codigo_op & 0x0F00) >> 8] = (codigo_op & 0x00FF) & (rand() % 0xFF);
			PC = PC + 2;
		break;

		case 0xD000: // 0xDXYN: Desenha um sprite 8XN nas coordenadas (Registrado[X], Registrador[Y])
					//Desenhar na tela atrav�s da opera��o XOR
					//Checa colis�o e marca o registrador [15]
					//L� a imagem da mem�ria
		{
			unsigned short x = registrador[(codigo_op & 0x0F00) >> 8];
			unsigned short y = registrador[(codigo_op & 0x00F0) >> 4];
			unsigned short altura = codigo_op & 0x000F;
			unsigned short pixel;
			registrador[15] = 0; //flag de colis�o

			for (int linha_y = 0; linha_y < altura; linha_y++)
			{
				pixel = memoria[i + linha_y];
				for (int linha_x = 0; linha_x < 8; linha_x++)
				{
					if ((pixel & (0x80 >> linha_x)) != 0)
					{
						int totalY = x + linha_x;
						int totalX = y + linha_y;
						int index = linha_y * 64 + totalX;

						if (gfx[index] == 1)
						{
							registrador[0xF] = 1;
						}
						gfx[index] ^= 1;
						//al_draw_filled_rectangle(x, y, linha_x, linha_y, black);
					}
				}
			}
			al_flip_display();
			flag_Tela = 1;
			PC = PC + 2;
		break;
		}
		case 0xE000:
			switch (codigo_op & 0x00FF)
			{
			case 0x009E: //0xEX9E: Pula a pr�xima instru��o se o registrador [X] for igual a tecla (pressionada)
				if (tecla[registrador[(codigo_op & 0x0F00) >> 8]] != 0)//Verifica
					PC += 4;
				else
					PC += 2;
				break;

			case 0x00A1: //0xEXA1: Pula a pr�xima instru��o se o registrador [X] for diferente da tecla (pressionada)
				if (tecla[registrador[(codigo_op & 0x0F00) >> 8]] == 0)//Verifica
					PC += 4;
				else
					PC += 2;
				break;

			default:
				printf("Codigo OP desconhecido [0x8000]: 0x%X\n", codigo_op);
				printf("Entrou no Switch das OPs 0xE000\n");
			}
		break;

		case 0xF000:
			switch (codigo_op & 0x00FF)
			{
				case 0x0007: // 0xFX07: Pega o valor do timer de delay e joga no Registrador[X]
					registrador[(codigo_op & 0x0F00) >> 8] = timer_delay;
					PC = PC + 2;
				break;

				/*case 0x000A: // 0xANNN: Quando uma tecla � pressionada a mesma � guardada no registrador [X]
				{
					al_wait_for_event(fila_eventos, &evento);

					if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
					{
						key_down();
						for (int i = 0; i < 16; i++)
						{
							if (tecla[i] != 0)
							{
								registrador[(codigo_op & 0x0F00) >> 8] = i;
							}
						}
					}	
					else 
					{
						return;
					}
					PC = PC + 2;
				}
				break;*/

				case 0x0015: // 0xFX15: Pega o valor do Registrador[X] e joga no timer de delay
					timer_delay = registrador[(codigo_op & 0x0F00) >> 8];
					PC = PC + 2;
				break;

				case 0x0018: // 0xFX18: Pega o valor do Registrador[X] e joga no timer de som
					timer_som = registrador[(codigo_op & 0x0F00) >> 8];
					PC = PC + 2;
				break;

				case 0x001E: // 0xFX1E: Soma o Registrador[X] + I e joga o resultado em I
					if (i + registrador[(codigo_op & 0x0F00) >> 8] > 0xFFF)	
						registrador[15] = 1;
					else
					{
						registrador[0] = 0;
					}

					i = i + registrador[(codigo_op & 0x0F00) >> 8];
					PC = PC + 2;
				break;

				case 0x0029: // 0xFX29: Pega o valor do timer de delay e joga no Registrador[X]
					i = i + registrador[(codigo_op & 0x0F00) >> 8];
					PC = PC + 2;
				break;

				case 0x0033: // 0xFX33: Guarda a representa��o bin�ria do valor no Registrador[X]
					memoria[i] = registrador[(codigo_op & 0x0F00) >> 8] / 100; //3 d�gitos mais significativos
					memoria[i + 1] = (registrador[(codigo_op & 0x0F00) >> 8] / 10) % 10; //d�gito do meio
					memoria[i + 2] = (registrador[(codigo_op & 0x0F00) >> 8] % 100) % 10; //D�gito menos significativo
					PC = PC + 2;
				break;

				case 0x0055: // 0xFX55: Guarda os dados do Registrador [0] ao Registrador[X] na mem�ria
					for (int contador = 0; contador <= registrador[(codigo_op & 0x0F00) >> 8]; contador++) {
						memoria[i] = registrador[contador];
						i = i + 1;
					}
					PC = PC + 2;
				break;

				case 0x0065: // 0xFX65: Pega os dados da mem�ria e guarda do Registrador [0] ao Registrador[X]
					for (int contador = 0; contador <= registrador[(codigo_op & 0x0F00) >> 8]; contador++) {
						registrador[contador] = memoria[i];
						i = i + 1;
					}
					PC = PC + 2;
				break;

				default:
					printf("Codigo OP desconhecido [0x8000]: 0x%X\n", codigo_op);
					printf("Entrou no Switch das OPs 0xF000\n");
			}
			break;

			default:
				printf("Codigo OP desconhecido [0x8000]: 0x%X\n", codigo_op);
				printf("Entrou no primeiro Switch\n");
		}

		// Atualiza os timers
		if (timer_delay > 0)
			timer_delay = timer_delay - 1;

		if (timer_som > 0)
		{
			if (timer_som == 1)
				al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); //Ativa o som
			timer_som = timer_som - 1;
			}
	}

bool chip8::carregarJogo(const char * filename)
{
	resetar();
	printf("Carregando: %s\n", filename);

	// Open file
	FILE * ROM = fopen(filename, "rb");
	if (ROM == NULL)
	{
		fputs("Erro no arquivo", stderr);
		return false;
	}

	// Check file size
	fseek(ROM, 0, SEEK_END);
	long lSize = ftell(ROM);
	rewind(ROM);
	printf("Tamanho do arquivo: %d\n", (int)lSize);

	// Reserva espa�o para a ROM na mem�ria
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL)
	{
		fputs("Erro na mem�ria.", stderr);
		return false;
	}

	// Copia a ROM para o buffer
	size_t result = fread(buffer, 1, lSize, ROM);
	if (result != lSize)
	{
		fputs("Erro na leitura.", stderr);
		return false;
	}

	// Copia o buffer para a mem�ria do CHIP-8
	if ((4096 - 512) > lSize)
	{
		for (int i = 0; i < lSize; ++i)
			memoria[i + 512] = buffer[i];
	}
	else
		printf("Error: A ROM � maior que a mem�ria.");

	// Fecha o arquivo e libera o buffer
	fclose(ROM);
	free(buffer);

	return true;
}

void chip8::key_down()
{
	if (ALLEGRO_KEY_1)		tecla[0x1] = 1;
	else if (ALLEGRO_KEY_2)	tecla[0x2] = 1;
	else if (ALLEGRO_KEY_3)	tecla[0x3] = 1;
	else if (ALLEGRO_KEY_4)	tecla[0xC] = 1;

	else if (ALLEGRO_KEY_Q)	tecla[0x4] = 1;
	else if (ALLEGRO_KEY_W)	tecla[0x5] = 1;
	else if (ALLEGRO_KEY_E)	tecla[0x6] = 1;
	else if (ALLEGRO_KEY_R)	tecla[0xD] = 1;

	else if (ALLEGRO_KEY_A)	tecla[0x7] = 1;
	else if (ALLEGRO_KEY_S)	tecla[0x8] = 1;
	else if (ALLEGRO_KEY_D)	tecla[0x9] = 1;
	else if (ALLEGRO_KEY_F)	tecla[0xE] = 1;

	else if (ALLEGRO_KEY_Z) tecla[0xA] = 1;
	else if (ALLEGRO_KEY_X)	tecla[0x0] = 1;
	else if (ALLEGRO_KEY_C)	tecla[0xB] = 1;
	else if (ALLEGRO_KEY_V)	tecla[0xF] = 1;
}

void chip8::key_up()
{
	if (ALLEGRO_KEY_1)		tecla[0x1] = 0;
	else if (ALLEGRO_KEY_2)	tecla[0x2] = 0;
	else if (ALLEGRO_KEY_3)	tecla[0x3] = 0;
	else if (ALLEGRO_KEY_4)	tecla[0xC] = 0;

	else if (ALLEGRO_KEY_Q)	tecla[0x4] = 0;
	else if (ALLEGRO_KEY_W)	tecla[0x5] = 0;
	else if (ALLEGRO_KEY_E)	tecla[0x6] = 0;
	else if (ALLEGRO_KEY_R)	tecla[0xD] = 0;

	else if (ALLEGRO_KEY_A)	tecla[0x7] = 0;
	else if (ALLEGRO_KEY_S)	tecla[0x8] = 0;
	else if (ALLEGRO_KEY_D)	tecla[0x9] = 0;
	else if (ALLEGRO_KEY_F)	tecla[0xE] = 0;

	else if (ALLEGRO_KEY_Z)	tecla[0xA] = 0;
	else if (ALLEGRO_KEY_X)	tecla[0x0] = 0;
	else if (ALLEGRO_KEY_C)	tecla[0xB] = 0;
	else if (ALLEGRO_KEY_V)	tecla[0xF] = 0;
}