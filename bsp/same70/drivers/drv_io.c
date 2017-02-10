#include <rtthread.h>
#include "board.h"

#include "drv_io.h"
#define PIN_PC_17  {PIO_PC17, PIOC, ID_PIOC, PIO_INPUT, PIO_IT_RISE_EDGE}
#define PIN_PC_10  {PIO_PC10, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define PIN_PD_4  	{PIO_PD4, PIOD, ID_PIOD, PIO_INPUT, PIO_DEFAULT}
#define PIN_PD_11  {PIO_PD11, PIOD, ID_PIOD, PIO_INPUT, PIO_DEFAULT}
#define PIN_PD_10  {PIO_PD10, PIOD, ID_PIOD, PIO_INPUT, PIO_DEFAULT}
#define PIN_PD_24  {PIO_PD24, PIOD, ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PA_24  {PIO_PA24, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PC_14  {PIO_PC14, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PD_9  {PIO_PD9, PIOD, ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PD_2  {PIO_PD2, PIOD, ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PC_16  {PIO_PC16, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PA_29  {PIO_PA29, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}

#define PINS_IO  {PIN_PC_17,PIN_PC_10,PIN_PD_4,PIN_PD_11,PIN_PD_10,PIN_PD_24,PIN_PA_24,PIN_PC_14,PIN_PD_9,PIN_PD_2,PIN_PC_16,PIN_PA_29}
typedef void (*callback_t)();
callback_t io_callback;

static const Pin pinsio[] = PINS_IO;

void io_output(int id, int value)
{
	if (value)
		PIO_Set(&pinsio[id]);
	else
		PIO_Clear(&pinsio[id]);
}
RTM_EXPORT(io_output);

int io_input(int id)
{
	return PIO_Get(&pinsio[id]);
}
RTM_EXPORT(io_input);

int io_init(callback_t callback_function)
{
	io_callback = callback_function;
	PIO_Configure(pinsio, PIO_LISTSIZE(pinsio));
	return 0;
}
RTM_EXPORT(io_init);

void PIOC_Handler(void)
{
	rt_interrupt_enter();
	io_callback();
	rt_interrupt_leave();
}
