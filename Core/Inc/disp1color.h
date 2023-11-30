//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _DISP1COLOR_H
#define _DISP1COLOR_H

//#include <types.h>

// Список поддерживаемых дисплеев (контроллеров дисплеев)
#define DISPTYPE_ssd1306        0       // OLED-дисплей с контроллером ssd1306
#define DISPTYPE_DMD_1color     1       // LED-матрица одноцветная на логике


// С каким типом дисплея будет работать модуль disp1color 
#define DISP1COLOR_type         DISPTYPE_DMD_1color

// Размеры дисплея в пикселях
#define DISP1COLOR_Width        32
#define DISP1COLOR_Height       16
#define DMD_1COLOR_MatrixHeight         16
#define DMD_1COLOR_MatrixWidth          32

#include <stdint.h>



// Процедура инициализирует 1-цветый дисплей
void disp1color_Init(void);
// Процедура управляет режимом Test дисплея
void disp1color_TestMode(uint8_t TestOn);
// Процедура устанавливает яркость дисплея
void disp1color_SetBrightness(uint8_t Value);
// Процедура заполняет буфер кадра значением FillValue
void disp1color_FillScreenbuff(uint8_t FillValue);
// Процедура обновляет состояние индикаторов в соответствии с буфером кадра
void disp1color_UpdateFromBuff(void);

// Процедура рисует прямую линию в буфере кадра дисплея
void disp1color_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
// Процедура рисует прямоугольник в буфере кадра дисплея
void disp1color_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
// Процедура рисует окружность в буфере кадра дисплея. x0 и y0 - координаты центра окружности
void disp1color_DrawCircle(int16_t x0, int16_t y0, int16_t radius);

// Функция вывода символа Char на дисплей. Возвращает ширину выведенного символа
uint8_t disp1color_DrawChar(int16_t X, int16_t Y, uint8_t FontID, uint8_t Char);
// Функция вывода текста из строки Str на дисплей
void disp1color_DrawString(int16_t X, int16_t Y, uint8_t FontID, uint8_t *Str);
// Процедура выводит на дисплей форматированную строку
void disp1color_printf(int16_t X, int16_t Y, uint8_t FontID, const char *args, ...);


//Вывод буфера с данными в консоль
void print_buf(uint8_t* buf, uint16_t size);
// Процедура передаёт в дисплей буфер кадра из массива pBuff
void DMD_1COLOR_DisplayFullUpdate(uint8_t *pBuff, uint16_t BuffLen);
// Процедура обновляет поочерёдно все матрицы экрана
void DMD_1COLOR_UpdateRow4(uint8_t Row4, uint8_t *pBuff, uint16_t BuffLen);
// Процедура обновляет 1 матрицу
void DMD_1COLOR_SendFromMatrixBuff(uint8_t Row4, uint8_t *pBuff);
// Функция возвращает байт (строку из 8 светодиодов) по 8-байтному фрагменту буфера
// графической библиотеки и маске соответствующей строки светодиодов во фрагменте
uint8_t DMD_1COLOR_GetFrom8ScreenBytes(uint8_t Mask, uint8_t *pBuff);
//Копирование из подготовленного буфера в массивиы для построчного вводы в матрицу
void prepare_data();
#endif
