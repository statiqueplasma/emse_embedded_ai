# WINE QUALITY PREDICTION

## TABLE OF CONTENT

[1. OVERVIEW](#overview)  

[2. MODEL TRAINING AND ADVERSARIAL ATTACKS](#model-training-and-adversarial-attacks)

[3. MODEL IMPORTATION AND IMPLEMENTATION](#model-importation-and-implementation)

[4. STM32 SERIAL COMMUNICATION](#stm32-serial-communication)

[5. CONCLUSION](#conclusion)

## OVERVIEW

This project focuses on the implementation of neural network model for wine quality prediction in a STM32L4R9 board, using Keras library to train the NN model, and X-Cube-AI extension of STM32Cube to implement the model on our board. And in the end we will do adversarial attacks to test the security and the safety of our model

## MODEL TRAINING AND ADVERSARIAL ATTACKS

The model training and attack are described in the Jupyter file "wine_quality_prediction.ipynb". Here we'll be focusing on the implementation and the test of the model on the board.

## MODEL IMPORTATION AND IMPLEMENTATION 
The first step is converting the NN model into an optimized code that can run on STM32 and implement it using the X-Cube-AI tool.

After that we have a generated software code containing all the files and the fuctions that we need to use the NN model on our board.

If we look at the  "app_x-cube-ai.c" file, we have functions that are essential in our project:

 - <b>"aquire_and_process_data"</b>: This function fills the input of the model
 - <b>"ai_run"</b> : This function process the data and calls the inference
 - <b>"post_process"</b> : This function post-processes the predictions and gives us the output classifications
 - <b>"MX_X_CUBE_AI_Process"</b> : This fuction calls the three functions above. It's the only function called in the main.

 Now we're goi,g to see in detail the content of each function

  -  <b>"aquire_and_process_data" :</b>

```c
int acquire_and_process_data(ai_i8* data[])
{

 /* fill the inputs of the c-model */

	uint8_t tmp[4] = {0}; //This buffer takes one feature at a time
	int i,k;

		#if _DEBUG
		float input[12] = {0};
		#endif



	  // Here we will receive all 12 feature of one sample
	  for (i = 0; i < 12; i++){
		  HAL_UART_Receive(&huart2, (uint8_t *) tmp, sizeof(tmp), 100);

		#if _DEBUG
		input[i] = *(float*) &tmp;
		#endif

	// Copy the received features in the "data" parameter
		  for ( k = 0; k < 4; k++){
			((uint8_t *) data)[(i*4)+k] = tmp[k];
		  }
	  }

	  return 0;
}
```
The function have one parameter "data", and returns 0.

We begin by creating a 32 bits buffer to store one feature, then we receive all the 12 features of each sample using "for" loop and via serial communication. Then we store the received features in the "data parameter"

 - <b>ai_run:</b>

 ```c
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
 ```

 The function don't have any parameter, and return 0 ( or -1 if we have an error)

 It simply call the "ai_wine_quality_run" function and gives it the model, and the input/output arrays, and store the return value in "batch". If the return value is "1", the model is succesfully run, otherwise we have a problem.

  - <b>post_process</b> 
```c
int post_process(ai_i8* data[])
{
/* process the predictions */

	  uint8_t *output = data; // put the "data parameter in a buffer"
	  int i,j;

	  // We wait for the synchronisation before sending the prediction
	  char sync[3] = "010";
	  HAL_UART_Transmit(&huart2, (uint8_t *) sync, sizeof(sync), 100);

	  //We send the prediction of each class
	  for(i=0; i<7; i++){
		  char tmp[4] = {0};
		  for (j=0; j < 4; j++){
		  		  tmp[j] = output[4*i+j];
		  	  }
		  HAL_UART_Transmit(&huart2, (uint8_t *) tmp, sizeof(tmp), 100);
	  }

	#ifdef _DEBUG
	  float predicted_quality = *(float*) &tmp;
	  printf("Predicted quality: %f\n", predicted_quality)
	#endif
	  return 0;
}
```
This function have one parameter "data", and returns 0.

We begin by creating an array buffer that takes the parameter's value, then we send a synchronisation message, It helps us to insure that the communication is ok, after that we send the prediction of each of the 7 classes that we have, one by one.

 - <b>MX_X_CUBE_AI_Process</b>
```c
void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
  int res = -1;
  uint8_t *in_data = NULL;
  uint8_t *out_data = NULL;
  printf("TEMPLATE - run - main loop\r\n");



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
      /* 1 - acquire and pre-process input data */
      res = acquire_and_process_data(in_data);
      /* 2 - process the data - call inference engine */
      if (res == 0)
        res = ai_run();
      /* 3- post-process the predictions */
      if (res == 0)
        res = post_process(out_data);
    } while (res==0);
  }

  if (res) {
    ai_error err = {AI_ERROR_INVALID_STATE, AI_ERROR_CODE_NETWORK};
    ai_log_err(err, "Process has FAILED");
  }
    /* USER CODE END 6 */
}
```
This function doesn't have any parameter and doesn't return any value. 

As we can see, we begin by declaring 2 buffers, then we affect to them the data field of "ai_input" and "ai_output" structures. after that we enter a while loop where we call the 3 previous functions as long as "ret = 0", which is the case all the time except when we have a problem.

This function is called in the while loop of the main.c file :
```c
 while (1)
  {
    /* USER CODE END WHILE */

  MX_X_CUBE_AI_Process();
    /* USER CODE BEGIN 3 */
  }
```
## STM32 SERIAL COMMUNICATION
After writing the code for executing our model on the board, now we need to give our model the input values and read the output prediction.

To do that, we made a python script that gives the model via UART communication, one sample with all the features, and receive the prediction of all the classes.

First of all, we load the test data that we will use using np.load function: 

```python
Y_data = np.load('WINE_QUALITY_Y_TEST.npy')
X_data = np.load('WINE_QUALITY_X_TEST.npy')
```

Then in our main function we'll initialize the serial communication

```python
ser = serial.Serial(
        # Prot used by your computer
        port='COM12',
        baudrate=115200,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    ) 
```

In our example we will send all 12 features of the 1st sample : 

```python
printProgressBar(0, 12, prefix = 'Sending Data:', suffix = 'Complete', length = 50)
for i in range(len(X_data[0])):
    out = ''
    ser.write((bytes(X_data[0][i])))
    printProgressBar(i+1, 12, prefix = 'Sending Data:', suffix = 'Complete', length = 50)
state = "receive"
```

Before receiving the prediction, we will make sure that we are synchronized between the PC and the board. For that the python script will wait to receive "010":

```python
sync = b"000"
while(sync != b"010"):
    sync = ser.read(3)
```

Finally, after a succesful synchronisation, we will now receive the prediction from the board :

```python

#Receive each prediction and store it in "output" array 
for i in range(7):
    prediction = struct.unpack('f', ser.read(4))[0]
    output[0][i] = prediction

#Print the prediction of each label 
if output[0][6] != 0:
    print("Expectation : ", Y_data[0])
    print("Predictions : \n\n")
    print("============================ \n")
    output_display = output.round(decimals=4)
    for i in range(7):
        print("-> Label : {} | prediction : {}\n".format(i,output_display[0][i]))
    print("============================ \n")
    state = "stop"
```

## CONCLUSION
This project enabled us to learn how to drive a NN model and embed it on an STM32 board. It also enabled us to learn the principle of adversarial attacks and their consequences. 

Unfortunately, the results that we had in the predictions weren't the results that we hoped for, but in general we succeeded in training the model, testing some adversarial attacks and their consequences, embed it in our board and write a python script to give the model input data and receive output data.






