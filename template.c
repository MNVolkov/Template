/*
	Application template for Amazfit Bip BipOS
	(C) Maxim Volkov  2019 <Maxim.N.Volkov@ya.ru>
	
	Шаблон приложения для загрузчика BipOS
	
*/

#include <libbip.h>
#include "template.h"

//	структура меню экрана - для каждого экрана своя
struct regmenu_ screen_data = {
						55,							//	номер главного экрана, значение 0-255, для пользовательских окон лучше брать от 50 и выше
						1,							//	вспомогательный номер экрана (обычно равен 1)
						0,							//	0
						dispatch_screen,			//	указатель на функцию обработки тача, свайпов, долгого нажатия кнопки
						key_press_screen, 			//	указатель на функцию обработки нажатия кнопки
						screen_job,					//	указатель на функцию-колбэк таймера 
						0,							//	0
						show_screen,				//	указатель на функцию отображения экрана
						0,							//	
						0							//	долгое нажатие кнопки
					};
int main(int param0, char** argv){	//	здесь переменная argv не определена
	show_screen((void*) param0);
}

void show_screen (void *param0){
struct app_data_** 	app_data_p = get_ptr_temp_buf_2(); 	//	указатель на указатель на данные экрана 
struct app_data_ *	app_data;					//	указатель на данные экрана

Elf_proc_* proc;

// проверка источника запуска процедуры
if ( (param0 == *app_data_p) && get_var_menu_overlay()){ // возврат из оверлейного экрана (входящий звонок, уведомление, будильник, цель и т.д.)

	app_data = *app_data_p;					//	указатель на данные необходимо сохранить для исключения 
											//	высвобождения памяти функцией reg_menu
	*app_data_p = NULL;						//	обнуляем указатель для передачи в функцию reg_menu	

	// 	создаем новый экран, при этом указатель temp_buf_2 был равен 0 и память не была высвобождена	
	reg_menu(&screen_data, 0); 				// 	menu_overlay=0
	
	*app_data_p = app_data;						//	восстанавливаем указатель на данные после функции reg_menu
	
	//   здесь проводим действия при возврате из оверлейного экрана: восстанавливаем данные и т.д.
	
	
} else { // если запуск функции произошел впервые т.е. из меню 

	// создаем экран (регистрируем в системе)
	reg_menu(&screen_data, 0);

	// выделяем необходимую память и размещаем в ней данные, (память по указателю, хранящемуся по адресу temp_buf_2 высвобождается автоматически функцией reg_menu другого экрана)
	*app_data_p = (struct app_data_ *)pvPortMalloc(sizeof(struct app_data_));
	app_data = *app_data_p;		//	указатель на данные
	
	// очистим память под данные
	_memclr(app_data, sizeof(struct app_data_));
	
	//	значение param0 содержит указатель на данные запущенного процесса структура Elf_proc_
	proc = param0;
	
	// запомним адрес указателя на функцию в которую необходимо вернуться после завершения данного экрана
	if ( param0 && proc->ret_f ) 			//	если указатель на возврат передан, то возвоащаемся на него
		app_data->ret_f = proc->elf_finish;
	else					//	если нет, то на циферблат
		app_data->ret_f = show_watchface;
	
	// здесь проводим действия которые необходимо если функция запущена впервые из меню: заполнение всех структур данных и т.д.
	
	app_data->col=0;
	
}

// здесь выполняем отрисовку интерфейса, обновление (перенос в видеопамять) экрана выполнять не нужно

draw_screen(app_data->col);

// при необходимости ставим таймер вызова screen_job в мс
set_update_period(1, 5000);
}

void key_press_screen(){
struct app_data_** 	app_data_p = get_ptr_temp_buf_2(); 	//	указатель на указатель на данные экрана 
struct app_data_ *	app_data = *app_data_p;				//	указатель на данные экрана

// вызываем функцию возврата (обычно это меню запуска), в качестве параметра указываем адрес функции нашего приложения
show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);	
};

void screen_job(){
// при необходимости можно использовать данные экрана в этой функции
struct app_data_** 	app_data_p = get_ptr_temp_buf_2(); 	//	указатель на указатель на данные экрана 
struct app_data_ *	app_data = *app_data_p;				//	указатель на данные экрана

// делаем периодическое действие: анимация, увеличение счетчика, обновление экрана,
// отрисовку интерфейса, обновление (перенос в видеопамять) экрана выполнять нужно

app_data->col = (app_data->col+1)%COLORS_COUNT;
draw_screen(app_data->col);

// перенос в видеопамять строк экрана, которые были перерисованы
repaint_screen_lines(0, 176);

// при необходимости заново ставим таймер вызова screen_job
set_update_period(1, 5000);
}

int dispatch_screen (void *param){
struct app_data_** 	app_data_p = get_ptr_temp_buf_2(); 	//	указатель на указатель на данные экрана 
struct app_data_ *	app_data = *app_data_p;				//	указатель на данные экрана

// в случае отрисовки интерфейса, обновление (перенос в видеопамять) экрана выполнять нужно



struct gesture_ *gest = param;
int result = 0;

switch (gest->gesture){
	case GESTURE_CLICK: {			
		if ( ( gest->touch_pos_y >66) &&  ( gest->touch_pos_y < 110) ){ 
			if ( ( gest->touch_pos_x >66) &&  ( gest->touch_pos_x < 110) ){
					// тач в центр экрана
					// выполняем действия
					vibrate(1,70,0);
					app_data->col = (app_data->col+1)%COLORS_COUNT;
					draw_screen(app_data->col);
					repaint_screen_lines(0, 176);
					}
			}
			break;
		};
		case GESTURE_SWIPE_RIGHT: {	//	свайп направо
			// обычно это выход из приложения
			show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);	
			break;
		};
		case GESTURE_SWIPE_LEFT: {	// справа налево
			// действия при свайпе влево	
			break;
		};
		case GESTURE_SWIPE_UP: {	// свайп вверх
			// действия при свайпе вверх
			break;
		};
		case GESTURE_SWIPE_DOWN: {	// свайп вниз
			// действия при свайпе вниз
			break;
		};		
		default:{	// что-то пошло не так...
			
			break;
		};		
		
	}
	
	return result;
};

// пользовательская функция
void draw_screen(int col){

static int colors_bg[COLORS_COUNT] = {COLOR_BLACK, COLOR_BLUE,  COLOR_RED, COLOR_PURPLE};
static int colors_fg[COLORS_COUNT] = {COLOR_AQUA,  COLOR_WHITE, COLOR_YELLOW, COLOR_GREEN};

set_bg_color(colors_bg[col]);
fill_screen_bg();
set_graph_callback_to_ram_1();
// подгружаем шрифты
load_font();
set_fg_color(colors_fg[col]);


text_out_center("Привет!!!", 88, 60);

};