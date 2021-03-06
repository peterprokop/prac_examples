/*
 * Пример магии по работе с терминалом от Алексея Сальникова.
 * Так сказать пособие для изучения для тех, 
 * кто пишет текстовый редактор.
 *
 */

/*
 * порядок инклюдов похоже важен.
 */
#include <termios.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>

int main()
{
    int symbol;

    struct termios old_attributes,new_attributes;
       

    printf("Данная программа делает терминал не копирующим свой ввод\n");
    printf("и не ожидающим нажатия <enter>\n\n");

    if(!isatty(0))
    {
        printf("Похоже вы перенаправили ввод не из терминала.\n");
        printf("В таких условиях работа программы безсмысленна.\n");
        return 1;  
    }
    
    /*
     * Собственно в man на ioctl (man tty_ioctl)
     * сказано, что ioctl чересчур операционно
     * системно зависимо, и стоит пользоваться вместо этого функциями
     * tcgetattr и tcsetattr.
     */
    tcgetattr(0,&old_attributes);

    /*
     * копируем атрибуты, чтобы потом 
     * восстановить исходное поведение терминала.
     */
    memcpy(&new_attributes,&old_attributes,sizeof(struct termios));
    
    /*
     * Теперь буковки набираемые не будут печататься.
     */
    new_attributes.c_lflag &= ~ECHO;
    
    /*
     * По идее теперь не нужно будет нажимать
     * <enter> для ввода символа. Однако,
     *  в результате ctrl+d тоже перестанет восприниматься 
     *  как конец файла.
     */
    new_attributes.c_lflag &= ~ICANON;

    /*
     * Выставляем, чтобы терминал считывал 
     * с клавиатуры по одному символу, 
     * включая стрелочки.
     */
    new_attributes.c_cc[VMIN] = 1;
    
    /* Включает жёсткое чтение по одному символу. 
     * Терминал будет работать не как сокет.
     * Поскольку как-то оно слишком агрессивно, по моему
     * мнению я это выключу нафик.
     */
    /* 
    new_attributes.c_cc[VTIME] = 0;
    */

    tcsetattr(0,TCSANOW,&new_attributes);

    printf("Пожалуйста вводите текст, настройки терминала уже изменены:\n");

    while((symbol=getchar())!=EOF)
    {
        printf("Был введён символ: код=%d начертание '%c'\n",symbol,(char)symbol);
        
        /*
         * О ужас, теперь, чтобы всё-таки сохранить иллюзию 
         * старого поведения на ctrl+d мы вынуждены делать такой хак.
         */
        if (symbol == '\004')
            break;
    }

    tcsetattr(0,TCSANOW,&old_attributes);

    printf("\nНастройки терминала изменены обратно\n");

    return 0;
}
