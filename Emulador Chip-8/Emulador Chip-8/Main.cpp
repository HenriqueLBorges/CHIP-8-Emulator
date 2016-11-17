#include "graficos_Allegro.h"
#include <allegro5/allegro.h>
#include <sstream>
#include <iostream>

using namespace std;


int main(int argc, char **argv)
{
	stringstream ss;
	chip8 chip_8;
	graficos_Allegro graficos;

	if (argc <= 2)
	{
		graficos = graficos_Allegro(5);
	}
	else if (argc == 3)
	{
		int width = 0;
		ss << argv[2];
		ss >> width;
		width /= 64;

		graficos = graficos_Allegro(width);
	}
	graficos.iniciar();
	chip_8.resetar();

	bool closeGame = false;

	if (argv[1] != NULL)
		chip_8.carregarJogo(argv[1]);

	while (!closeGame)
	{

		chip_8.emula_Ciclo();
		closeGame = graficos.input(chip_8);
	}

	return 0;


	/*if (!chip_8.inicializar()) {
		return -1;
	}
	if (!chip_8.carregarJogo(argv[1])) {
		return -1;
	}

	bool encerrar = false;
	while (!encerrar) {
		chip_8.emula_Ciclo();
		if (chip_8.flag_Tela == 1)
		{
			al_flip_display();
		}
		if (chip_8.evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			encerrar = true;
		}
	}
	return 0;*/
}


