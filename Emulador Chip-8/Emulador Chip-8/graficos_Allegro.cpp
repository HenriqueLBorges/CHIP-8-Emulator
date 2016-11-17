#include "graficos_Allegro.h"


graficos_Allegro::graficos_Allegro()
{
	FPS = 200;
	display = NULL;
	fila_eventos = NULL;
	timer = NULL;
	redesenhar = true;
	largura = 64;
	altura = 32;
	modificador = 1;
	pixel = NULL;
}

graficos_Allegro::graficos_Allegro(int mod)
{
	modificador = mod;
	FPS = 200;
	display = NULL;
	fila_eventos = NULL;
	timer = NULL;
	redesenhar = true;
	largura = 64 * modificador;
	altura = 32 * modificador;
	pixel = NULL;
}

bool graficos_Allegro::iniciar()
{
	if (!al_init())
	{
		fprintf_s(stderr, "ERRO - Falha ao inicializar a biblioteca Allegro!\n");
		return false;
	}

	if (!al_install_keyboard())
	{
		fprintf_s(stderr, "ERRO - Falha ao inicializar o teclado!\n");
		return false;
	}

	timer = al_create_timer(1.0 / FPS);
	if (!timer)
	{
		fprintf_s(stderr, "ERRO - Falha ao criar o timer!\n");
		return false;
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_DIRECT3D_INTERNAL);

	display = al_create_display(largura, altura);
	if (!display)
	{
		fprintf_s(stderr, "ERRO - Falha ao criar o display!\n");
		al_destroy_timer(timer);
		return false;
	}




	fila_eventos = al_create_event_queue();
	if (!fila_eventos)
	{
		fprintf_s(stderr, "ERRO - Falha ao criar a fila de eventos!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return false;
	}

	al_init_primitives_addon();

	al_register_event_source(fila_eventos, al_get_display_event_source(display));
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());

	pixel = al_create_bitmap(modificador, modificador);
	al_set_target_bitmap(pixel);
	al_draw_filled_rectangle(0, 0, modificador, modificador, al_map_rgb(255, 255, 255));

	al_set_target_bitmap(al_get_backbuffer(display));
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	al_start_timer(timer);


	return true;
}

bool graficos_Allegro::input(chip8& chip8)
{
	ALLEGRO_EVENT ev;
	al_wait_for_event(fila_eventos, &ev);

	if (ev.type == ALLEGRO_EVENT_TIMER)
	{
		chip8.flag_Tela = true;
	}
	else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
	{
		return limpar();
	}
	else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
	{

		if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)    // esc
			return limpar();

		if (ev.keyboard.keycode == ALLEGRO_KEY_1)		chip8.tecla[0x1] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_2)	chip8.tecla[0x2] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_3)	chip8.tecla[0x3] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_4)	chip8.tecla[0xC] = 1;

		else if (ev.keyboard.keycode == ALLEGRO_KEY_Q)	chip8.tecla[0x4] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_W)	chip8.tecla[0x5] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_E)	chip8.tecla[0x6] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_R)	chip8.tecla[0xD] = 1;

		else if (ev.keyboard.keycode == ALLEGRO_KEY_A)	chip8.tecla[0x7] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_S)	chip8.tecla[0x8] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_D)	chip8.tecla[0x9] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_F)	chip8.tecla[0xE] = 1;

		else if (ev.keyboard.keycode == ALLEGRO_KEY_Y)	chip8.tecla[0xA] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_X)	chip8.tecla[0x0] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_C)	chip8.tecla[0xB] = 1;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_V)	chip8.tecla[0xF] = 1;
		// keycode stuff set to true
	}
	else if (ev.type == ALLEGRO_EVENT_KEY_UP)
	{
		if (ev.keyboard.keycode == ALLEGRO_KEY_1)		chip8.tecla[0x1] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_2)	chip8.tecla[0x2] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_3)	chip8.tecla[0x3] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_4)	chip8.tecla[0xC] = 0;

		else if (ev.keyboard.keycode == ALLEGRO_KEY_Q)	chip8.tecla[0x4] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_W)	chip8.tecla[0x5] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_E)	chip8.tecla[0x6] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_R)	chip8.tecla[0xD] = 0;

		else if (ev.keyboard.keycode == ALLEGRO_KEY_A)	chip8.tecla[0x7] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_S)	chip8.tecla[0x8] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_D)	chip8.tecla[0x9] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_F)	chip8.tecla[0xE] = 0;

		else if (ev.keyboard.keycode == ALLEGRO_KEY_Y)	chip8.tecla[0xA] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_X)	chip8.tecla[0x0] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_C)	chip8.tecla[0xB] = 0;
		else if (ev.keyboard.keycode == ALLEGRO_KEY_V)	chip8.tecla[0xF] = 0;
		// keycode stuff set to false
	}

	if (chip8.flag_Tela && al_is_event_queue_empty(fila_eventos))
	{
		chip8.flag_Tela = false;

		al_clear_to_color(al_map_rgb(0, 0, 0));

		for (int y = 0; y < 32; ++y)
			for (int x = 0; x < 64; ++x)
			{
				if (!chip8.gfx[(y * 64) + x] == 0)
				{
					al_draw_bitmap(pixel, x*modificador, y*modificador, ALLEGRO_VIDEO_BITMAP);
				}
			}


		al_flip_display();
	}

	return false;
}

bool graficos_Allegro::limpar()
{
	al_destroy_bitmap(pixel);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(fila_eventos);

	return true;
}