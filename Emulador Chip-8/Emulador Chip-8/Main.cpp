#include "chip8.h"
#include <allegro5/allegro.h>

chip8 chip_8;


int main(int argc, char **argv)
{
	ALLEGRO_EVENT evento;
	al_wait_for_event(chip_8.fila_eventos, &evento);

	if (!chip_8.inicializar()) {
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
		if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			encerrar = true;
		}
	}
	return 0;
}


