/*
	Application template for Amazfit Bip BipOS
	(C) Maxim Volkov  2019 <Maxim.N.Volkov@ya.ru>
	
	Шаблон приложения, заголовочный файл

*/

#ifndef __APP_TEMPLATE_H__
#define __APP_TEMPLATE_H__

#define COLORS_COUNT	4

// структура данных для нашего экрана
struct app_data_ {
			void* 	ret_f;					//	адрес функции возврата
			int		col;					//	текущий цвет надписи
};

// template.c
void 	show_screen (void *return_screen);
void 	key_press_screen();
int 	dispatch_screen (void *param);
void 	screen_job();
void	draw_screen(int col);
#endif