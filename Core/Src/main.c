/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "disp1color.h"
#include "stdlib.h"
#include "time.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t data1[16], data2[16], data3[16], data4[16];

volatile int16_t gridX, gridY;
uint8_t grid[8][16];
uint8_t grid_buff[16][8];
char piece_pool[] = {'Z', 'S', 'L', 'J', 'O', 'T', 'I'};
char piece;
uint8_t piece_matrix[4][4];
uint8_t new_turn = 1;
volatile uint8_t moved_down = 0, moved_left = 0, moved_right = 0;
volatile uint8_t rotated, hard_drop;
int8_t i,j;
uint8_t game_over = 0;
uint16_t adc_noise;

uint8_t inside_play_field(){
	return (gridX+i>=0)&&(gridX+i<8)&&(gridY+j<16)&&(gridY+j>=0);
}


void generate_piece(char piece){
	for(i = 0; i < 4;i++){
		for(j = 0; j < 4; j++){
			piece_matrix[i][j] = 0;
		}
	}
	switch (piece){
		case 'Z':{
			piece_matrix[0][1] = 1;
			piece_matrix[1][1] = 1;
			piece_matrix[1][2] = 1;
			piece_matrix[2][2] = 1;
//			 {{0, 0, 0, 0},
//		  	  {1, 1, 0, 0},
//		 	  {0, 1, 1, 0},
//			  {0, 0, 0, 0}};
			break;
		}
		case 'S':{
			piece_matrix[1][1] = 1;
			piece_matrix[2][1] = 1;
			piece_matrix[0][2] = 1;
			piece_matrix[1][2] = 1;
//			 {{0, 0, 0, 0},
//	  		  {0, 1, 1, 0},
//	 		  {1, 1, 0, 0},
// 			  {0, 0, 0, 0}};
		}
			break;
		case 'L':{
			piece_matrix[1][0] = 1;
			piece_matrix[1][1] = 1;
			piece_matrix[1][2] = 1;
			piece_matrix[2][2] = 1;
//			 {{0, 1, 0, 0},
//	  		  {0, 1, 0, 0},
//	 		  {0, 1, 1, 0},
// 			  {0, 0, 0, 0}};
			break;
		}
		case 'J':{
			piece_matrix[2][0] = 1;
			piece_matrix[2][1] = 1;
			piece_matrix[1][2] = 1;
			piece_matrix[2][2] = 1;
//			 {{0, 0, 1, 0},
//	  		  {0, 0, 1, 0},
//	 		  {0, 1, 1, 0},
// 			  {0, 0, 0, 0}};
			break;
		}
		case 'O':{
			piece_matrix[1][1] = 1;
			piece_matrix[2][1] = 1;
			piece_matrix[1][2] = 1;
			piece_matrix[2][2] = 1;
//			 {{0, 0, 0, 0},
//	  		  {0, 1, 1, 0},
//	 		  {0, 1, 1, 0},
// 			  {0, 0, 0, 0}};
			break;
		}
		case 'T':{
			piece_matrix[0][1] = 1;
			piece_matrix[1][1] = 1;
			piece_matrix[2][1] = 1;
			piece_matrix[1][2] = 1;
//			 {{0, 0, 0, 0},
//	  		  {1, 1, 1, 0},
//	 		  {0, 1, 0, 0},
// 			  {0, 0, 0, 0}};
			break;
		}
		case 'I':{
			piece_matrix[1][0] = 1;
			piece_matrix[1][1] = 1;
			piece_matrix[1][2] = 1;
			piece_matrix[1][3] = 1;
//			 {{0, 1, 0, 0},
//	  		  {0, 1, 0, 0},
//	 		  {0, 1, 0, 0},
// 			  {0, 1, 0, 0}};
			break;
		}
	}
}

void rotate_clockwise(uint8_t matrix[4][4])
{
    int i, j, temp;

    // Transpose the matrix in place
    for (i = 0; i < 4; i++) {
        for (j = i + 1; j < 4; j++) {
            temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }

    // Reverse the rows of the transposed matrix
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            temp = matrix[i][j];
            matrix[i][j] = matrix[3 - i][j];
            matrix[3 - i][j] = temp;
        }
    }
}

void rotate_counter_clockwise(uint8_t matrix[4][4])
{
    int i, j, temp;

    // Reverse the columns of the matrix
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            temp = matrix[i][j];
            matrix[i][j] = matrix[i][3 - j];
            matrix[i][3 - j] = temp;
        }
    }

    // Transpose the matrix in place
    for (i = 0; i < 4; i++) {
        for (j = i + 1; j < 4; j++) {
            temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }
}

void delete_old_piece(int16_t X, int16_t Y){
	  for(i = 0; i<4; i++){
		  for(j = 0; j<4; j++){
			  if (piece_matrix[i][j]&&inside_play_field());
			  grid[X+i][Y+j] = 0;
	  			  }
	  		  }
}

void disp_row(int row){



	  if (row == 0){

		  		HAL_SPI_Transmit(&hspi1, data1, 16, 10);
		  		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);

		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET);

		  HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_RESET);
	  }
	  if (row == 1){

	  		HAL_SPI_Transmit(&hspi1, data2, 16, 10);
	  		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);

		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET);

		  HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_RESET);

	  }

	  if (row == 2){

	  		HAL_SPI_Transmit(&hspi1, data3, 16, 10);
	  		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);

		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET);

		  HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);

	  }

	  if (row == 3){

	  		HAL_SPI_Transmit(&hspi1, data4, 16, 10);
	  		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);

		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET);

		  HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);

	  }


	  HAL_GPIO_WritePin(nOE_GPIO_Port, nOE_Pin, GPIO_PIN_SET);
		  for(uint32_t x=0; x<=500; x++) {};
	 HAL_GPIO_WritePin(nOE_GPIO_Port, nOE_Pin, GPIO_PIN_RESET);
  }

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
 HAL_GPIO_WritePin(nOE_GPIO_Port, nOE_Pin, GPIO_PIN_RESET);
 HAL_TIM_Base_Start_IT(&htim3);
 //initialize grid
 for (i = 0; i < 8; i++){
	 for(j = 0; j < 16; j++){
		 grid[i][j] = 0;
	 }
 }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  while(!game_over){
		  if(new_turn){
			  //-----------
			  //clear lines
			  //-----------

			  //check for lines to clear
			  uint8_t line_to_clear[16];
			  uint8_t temp_line = 1;
			  for(j = 0; j < 16; j++){
				  for(i = 0; i < 8; i++){
					  temp_line = temp_line*grid[i][j];
				  }
				  if (temp_line) line_to_clear[j] = 1;
				  else line_to_clear[j] = 0;
				  temp_line = 1;
			  }

			  //delete lines on grid
			  for (int8_t line = 15; line >= 0; line--){
				  if(line_to_clear[line]){
					for(j = line; j >= 0; j--){
					  for(i = 0; i < 8; i++){
						  if (j == 0) grid[i][j] = 0;
						  else grid[i][j] = grid[i][j-1];
					  }
					}

					//move everything down
					for(int8_t line_fix = 15; line_fix >= 0; line_fix--){
						if (line_fix == 0) line_to_clear[line_fix] = 0;
						else line_to_clear[line_fix] = line_to_clear[line_fix-1];
					}

					if(line_to_clear[line]) line++;
				  }
			  }

			  HAL_ADC_Start(&hadc1);
			  HAL_ADC_PollForConversion(&hadc1, 1);
			  adc_noise = HAL_ADC_GetValue(&hadc1);
			  piece = piece_pool[(rand()+adc_noise)%7];
			  generate_piece(piece);
			  gridX = 2;
			  gridY = -3;
			  new_turn = 0;
			  moved_down = 0;
			  moved_left = 0;
			  moved_right = 0;
			  hard_drop = 0;
		  }

		  //check for every event
		  if(moved_down){
			  //check for collision with grid
						uint8_t floor_collided = 0;
						for(i = 0; i < 4; i++){
							for(j= 0; j < 4; j++){
								if (gridY + j >= 16) floor_collided += piece_matrix[i][j]*1;
								else if (inside_play_field()) floor_collided += piece_matrix[i][j]*grid[gridX+i][gridY+j];
								if((piece_matrix[i][j]&&grid[gridX+i][gridY+j]) && (gridY+j == 0)) game_over = 1;
							}
						}
						if (floor_collided) {
							new_turn = 1;
							gridY--;
							//save grid
							  for(i = 0; i<4; i++){
								  for(j = 0; j<4; j++){
									  if(inside_play_field())
									  grid[gridX+i][gridY+j] = grid[gridX+i][gridY+j]||piece_matrix[i][j];
								  }
							  }
						}
			  moved_down = 0;
		  }

		  if(hard_drop){
			  //move piece down until collided
			  	uint8_t floor_collided = 0;
			  	while(!floor_collided){
				  gridY++;
				  for(i = 0; i < 4; i++){
					  for(j= 0; j < 4; j++){
							if (gridY + j >= 16) floor_collided += piece_matrix[i][j]*1;
							else if (inside_play_field()) floor_collided += piece_matrix[i][j]*grid[gridX+i][gridY+j];
							if((piece_matrix[i][j]&&grid[gridX+i][gridY+j]) && (gridY+j == 0)) game_over = 1;
					  }
				  }
			  	}
			  	new_turn = 1;
				gridY--;
				//save grid
				  for(i = 0; i<4; i++){
					  for(j = 0; j<4; j++){
						  if(inside_play_field())
						  grid[gridX+i][gridY+j] = grid[gridX+i][gridY+j]||piece_matrix[i][j];
					  }
				  }
				hard_drop = 0;
		  }

		  if(moved_left){
			  //check for collision with anything on the left
				uint8_t left_wall_collided = 0;
				for(i = 0; i < 4; i++){
					for(j= 0; j < 4; j++){
						if (gridX + i < 0) left_wall_collided += piece_matrix[i][j]*1;
						else if (inside_play_field()) left_wall_collided += piece_matrix[i][j]*grid[gridX+i][gridY+j];
					}
				}
				if (left_wall_collided) {
					gridX++;
				}
			  moved_left = 0;
		  }

		  if(moved_right){
			  //check for collision with right wall
				uint8_t right_wall_collided = 0;
				for(i = 0; i < 4; i++){
					for(j= 0; j < 4; j++){
						if (gridX + i >= 8) right_wall_collided += piece_matrix[i][j]*1;
						else if (inside_play_field()) right_wall_collided += piece_matrix[i][j]*grid[gridX+i][gridY+j];
					}
				}
				if (right_wall_collided) {
					gridX--;
				}
			  moved_right = 0;
		  }

		  if(rotated){
			  //create a temporary copy
			  uint8_t temp[4][4];
			  for(i = 0; i < 4; i++){
				  for(j = 0; j < 4; j++){
					  temp[i][j] = piece_matrix[i][j];
				  }
			  }
			  rotate_clockwise(piece_matrix);
			  //check for all collisions
				uint8_t collided = 0;
				for(i = 0; i < 4; i++){
					for(j= 0; j < 4; j++){
						if ((gridX + i >= 8) || (gridX + i < 0) || (gridY + j < 0) || (gridY + j >= 16))
							collided += piece_matrix[i][j]*1;
						else if (inside_play_field()) collided += piece_matrix[i][j]*grid[gridX+i][gridY+j];
					}
				}
				if (collided) {
					//roll back piece matrix
					for(i = 0; i < 4; i++){
						  for(j = 0; j < 4; j++){
							  piece_matrix[i][j] = temp[i][j];
						  }
					}
					rotate_counter_clockwise(piece_matrix);
					//check for all collisions again
						uint8_t collided = 0;
						for(i = 0; i < 4; i++){
							for(j= 0; j < 4; j++){
								if ((gridX + i >= 8) || (gridX + i < 0) || (gridY + j >= 16))
									collided += piece_matrix[i][j]*1;
								else if (inside_play_field()) collided += piece_matrix[i][j]*grid[gridX+i][gridY+j];
							}
						}
						if (collided){
							//roll back piece matrix
							for(i = 0; i < 4; i++){
								  for(j = 0; j < 4; j++){
									  piece_matrix[i][j] = temp[i][j];
								  }
							}
						}
				}
			  rotated = 0;

		  }

		  //create temporary grid for buffering
		  uint8_t temp_grid[8][16];
		  for(i = 0; i < 8; i++){
			  for(j = 0; j < 16; j++){
				  temp_grid[i][j] = grid[i][j];
			  }
		  }
		  //setting current piece's place on temp grid
		  for(i = 0; i<4; i++){
			  for(j = 0; j<4; j++){
				  if(inside_play_field())
				  temp_grid[gridX+i][gridY+j] = temp_grid[gridX+i][gridY+j]||piece_matrix[i][j];
			  }
		  }
		  //transpose the temp grid matrix for buffering
		  for(i = 0; i < 8; i++){
			  for(j = 0; j < 16; j++){
				  grid_buff[j][i] = temp_grid[i][j];
			  }
		  }
		  //reverse the y axis of transposed matrix
		  for(i = 0; i < 16; i++){
			  for(j = 0; j < 4; j++){
				  uint8_t temp = grid_buff[i][j];
				  grid_buff[i][j] = grid_buff[i][7-j];
				  grid_buff[i][7-j] = temp;
			  }
		  }

		//  draw pixels on the display buffer
		  disp1color_FillScreenbuff(0);
		  for(int16_t dot_x = 0; dot_x < 16; dot_x++){
			  for(int16_t dot_y = 0; dot_y < 8; dot_y++){
				  if (grid_buff[dot_x][dot_y])
				  disp1color_DrawRectangle(2*dot_x, 2*dot_y, 2*dot_x + 1, 2*dot_y + 1);
			  }
		  }

	//	  	  disp1color_DrawRectangle(5,5,10,10);
			  disp1color_UpdateFromBuff();
			  prepare_data();

			  disp_row(0);
			  disp_row(1);
			  disp_row(2);
			  disp_row(3);

	  }
	  //clear grid
	  for(i = 0; i < 8; i++){
		  for(j = 0; j < 16; j++){
			  grid[i][j] = 0;
		  }
	  }
	  HAL_Delay(3000);
	  new_turn = 1;
	  game_over = 0;
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
