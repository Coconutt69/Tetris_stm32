# Description
 Tetris in STM32 with P10 LED matrix, controls using Arduino Esplora.

To run open .cproject file with STM32CubeIDE. Board used: Discovery Kit with STM32F411VET6.

# LED matrix

![image](https://github.com/Coconutt69/Tetris_stm32/assets/137206541/9b28e373-9789-4950-94fe-bff5be41cc47)

The full electrical scheme of the matrix:

![Pasted image 20231201044359](https://github.com/Coconutt69/Tetris_stm32/assets/137206541/139554af-ff49-40c4-9218-2e59afcabf14)

P10 LED matrix has 16 registers (D1-16), each controlling 8 LEDs. The two inputs A and B is used to choose which lines of LED are connected to the registers, resulting in 4 interpolated arrays of LEDs. The full image is drawn on the matrix by quickly switching between these 4 arrays. 

Connection to the STM:

![image](https://github.com/Coconutt69/Tetris_stm32/assets/137206541/2453f74f-7eb5-4e7c-909a-bfaefc413316)

# Esplora 

The buttons on the Arduino Esplora is used for controlling in the game. This can be replaced by just normal buttons to GPIO. If you want to use the same then load the tetris_joystick.ino file to the Esplora before connecting. 

![image](https://github.com/Coconutt69/Tetris_stm32/assets/137206541/0dcf7cd9-bc07-4e35-82d0-e99fc7db7520)

# Result

![image](https://github.com/Coconutt69/lab5/assets/137206541/bb62fed5-ce2f-4402-a7ea-f3809cac5dd5)
