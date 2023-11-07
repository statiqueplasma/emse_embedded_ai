
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Embedded Client API
  *          model. Only one input and one output is supported. All
  *          memory resources are allocated statically (AI_NETWORK_XX, defines
  *          are used).
  *          Re-target of the printf function is out-of-scope.
  *   v2.0 - add multiple IO and/or multiple heap support
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\<user_name>\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#if defined ( __ICCARM__ )
#elif defined ( __CC_ARM ) || ( __GNUC__ )
#endif

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"
#include "ai_datatypes_defines.h"
#include "wine_quality.h"
#include "wine_quality_data.h"

/* USER CODE BEGIN includes */
 extern UART_HandleTypeDef huart2;
/* USER CODE END includes */

/* IO buffers ----------------------------------------------------------------*/

#if !defined(AI_WINE_QUALITY_INPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_in_1[AI_WINE_QUALITY_IN_1_SIZE_BYTES];
ai_i8* data_ins[AI_WINE_QUALITY_IN_NUM] = {
data_in_1
};
#else
ai_i8* data_ins[AI_WINE_QUALITY_IN_NUM] = {
NULL
};
#endif

#if !defined(AI_WINE_QUALITY_OUTPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_out_1[AI_WINE_QUALITY_OUT_1_SIZE_BYTES];
ai_i8* data_outs[AI_WINE_QUALITY_OUT_NUM] = {
data_out_1
};
#else
ai_i8* data_outs[AI_WINE_QUALITY_OUT_NUM] = {
NULL
};
#endif

/* Activations buffers -------------------------------------------------------*/

AI_ALIGNED(32)
static uint8_t pool0[AI_WINE_QUALITY_DATA_ACTIVATION_1_SIZE];

ai_handle data_activations0[] = {pool0};

/* AI objects ----------------------------------------------------------------*/

static ai_handle wine_quality = AI_HANDLE_NULL;

static ai_buffer* ai_input;
static ai_buffer* ai_output;

static void ai_log_err(const ai_error err, const char *fct)
{
  /* USER CODE BEGIN log */
  if (fct)
    printf("TEMPLATE - Error (%s) - type=0x%02x code=0x%02x\r\n", fct,
        err.type, err.code);
  else
    printf("TEMPLATE - Error - type=0x%02x code=0x%02x\r\n", err.type, err.code);

  do {} while (1);
  /* USER CODE END log */
}

static int ai_boostrap(ai_handle *act_addr)
{
  ai_error err;

  /* Create and initialize an instance of the model */
  err = ai_wine_quality_create_and_init(&wine_quality, act_addr, NULL);
  if (err.type != AI_ERROR_NONE) {
    ai_log_err(err, "ai_wine_quality_create_and_init");
    return -1;
  }

  ai_input = ai_wine_quality_inputs_get(wine_quality, NULL);
  ai_output = ai_wine_quality_outputs_get(wine_quality, NULL);

#if defined(AI_WINE_QUALITY_INPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-inputs" option is used, memory buffer can be
   *  used from the activations buffer. This is not mandatory.
   */
  for (int idx=0; idx < AI_WINE_QUALITY_IN_NUM; idx++) {
	data_ins[idx] = ai_input[idx].data;
  }
#else
  for (int idx=0; idx < AI_WINE_QUALITY_IN_NUM; idx++) {
	  ai_input[idx].data = data_ins[idx];
  }
#endif

#if defined(AI_WINE_QUALITY_OUTPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-outputs" option is used, memory buffer can be
   *  used from the activations buffer. This is no mandatory.
   */
  for (int idx=0; idx < AI_WINE_QUALITY_OUT_NUM; idx++) {
	data_outs[idx] = ai_output[idx].data;
  }
#else
  for (int idx=0; idx < AI_WINE_QUALITY_OUT_NUM; idx++) {
	ai_output[idx].data = data_outs[idx];
  }
#endif

  return 0;
}

static int ai_run(void)
{
  ai_i32 batch;

  batch = ai_wine_quality_run(wine_quality, ai_input, ai_output);
  if (batch != 1) {
    ai_log_err(ai_wine_quality_get_error(wine_quality),
        "ai_wine_quality_run");
    return -1;
  }

  return 0;
}

/* USER CODE BEGIN 2 */
int acquire_and_process_data(ai_i8* data[])
{
 /* fill the inputs of the c-model */
	uint8_t tmp[4] = {0};

	#if _DEBUG
	float input[12] = {0}; // Stocker les valeurs d'entrée en mode Debug
	#endif

	  int i,k;
	  for (i = 0; i < 13; i++){
		  HAL_UART_Receive(&huart2, (uint8_t *) tmp, sizeof(tmp), 100);

	#if _DEBUG
	input[i] = *(float*) &tmp;
	#endif

	// Copier les bytes reçus dans le tableau de données
		  for ( k = 0; k < 4; k++){
			((uint8_t *) data)[((i*8)*4)+k] = tmp[k];
		  }
	  }

	#if _DEBUG

	  // Transmettre à nouveau les valeurs reçues en mode debug
	for(i = 0; i < 8; i++){
		float value = input[i];
		for (k = 0; k < 4 ; k++){
		   tmp[k] = ((uint8_t *) &value)[k];
		}
		HAL_UART_Transmit(&huart2, (uint8_t *) tmp, sizeof(tmp), 100);
	}

	#endif

	  return 0;
}

int post_process(ai_i8* data[])
{
/* process the predictions */
	  unsigned char output_to_be_tx[3] = "101";
	  uint8_t *output = data; // don't care about the signed value of ai_i8...
	  int i;

	  uint8_t tmp[4] = {0};

	  for (i=0; i < 4; i++){
		  tmp[i] = output[i];
	  }

	#ifdef _DEBUG
	  float predicted_quality = *(float*) &tmp;
	  printf("Predicted quality: %f\n", predicted_quality)
	#endif

	HAL_UART_Transmit(&huart2, (uint8_t *) output_to_be_tx, sizeof(output_to_be_tx),100);

	for (i=0; i < 4; i++){
		tmp[i] = output[i];
	}
	HAL_UART_Transmit(&huart2, (uint8_t *) tmp, sizeof(tmp), 100);
	  return 0;
}
/* USER CODE END 2 */

/* Entry points --------------------------------------------------------------*/

void MX_X_CUBE_AI_Init(void)
{
    /* USER CODE BEGIN 5 */
  printf("\r\nTEMPLATE - initialization\r\n");

  ai_boostrap(data_activations0);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
  int res = -1;
  uint8_t *in_data = NULL;
  uint8_t *out_data = NULL;
  printf("TEMPLATE - run - main loop\r\n");

  char test[3] = "101";
  char Rx[2];

  if (wine_quality) {
  // set pointer on NN buffer
	#if defined(AI_WINE_QUALITY_INPUTS_IN_ACTIVATIONS)
	  in_data = ai_input[0].data;
	#else
	  in_data = in_data_s;
	#endif

	#if defined(AI_WINE_QUALITY_OUTPUTS_IN_ACTIVATIONS)
	  out_data = ai_output[0].data;
	#else
	  out_data = out_data_s;
	#endif

    do {
    	HAL_UART_Receive(&huart2, (uint8_t *) Rx, sizeof(Rx), 100);

    	if (Rx[1] == 'o' && Rx[0] == 'k'){
    		HAL_UART_Transmit(&huart2, (uint8_t *) test, sizeof(test), 100);
    	}
      /* 1 - acquire and pre-process input data */
      res = acquire_and_process_data(data_ins);
      /* 2 - process the data - call inference engine */
      if (res == 0)
        res = ai_run();
      /* 3- post-process the predictions */
      if (res == 0)
        res = post_process(data_outs);
    } while (res==0);
  }

  if (res) {
    ai_error err = {AI_ERROR_INVALID_STATE, AI_ERROR_CODE_NETWORK};
    ai_log_err(err, "Process has FAILED");
  }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
