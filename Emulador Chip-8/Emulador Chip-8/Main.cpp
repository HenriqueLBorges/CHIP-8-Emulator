#include "chip8.h"
#include <allegro5/allegro.h>

chip8 chip_8;


int main(int argc, char **argv)
{
	if (!chip_8.inicializar()) {
		return -1;
	}
	if (!chip_8.carregarJogo(argv[1])) {
		return -1;
	}
	bool encerrar = false;
	while (!encerrar) {
		if (ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			encerrar = true;
		}
		chip_8.emula_Ciclo();

		if (chip_8.flag_Tela)
		{

		}
	}
	return 0;
}


