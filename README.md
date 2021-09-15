### Introduction
Etch-a-STMetch is a toy based on the famous Etch-a-Sketch toy. This project was used as a semestral work project for a course on my university. 

### Controls
The colour of the cursor is set by three potentiometers for each of the RGB colours. These are read approximately 5 times a second.

The position of the cursor is controlled by two rotary encoders in a similiar fashion as the original toy. The encoders are read by interrupts on order to reliably read the user input.

The modes of the cursor are set by two SPDT switches: 
>	 - "Pohyb" - the cursor moves without editing the image 
>	- "Kresleni" - the cursor colours the pixel it moves to with the correspnding colour.
>	 - "Mazani" - the cursor is set to be black and "removes" other colours. This mode has a higher priority than the others.

### Additional details
The toy communicates via Serial, so a Serial-to-USB converter is needed. The baud rate is 460800 to ensure the highest possible redraw speed. 

The font choice can greatly affect the user experience - a big font with square space character works best. Such font is included in this repo. If another font is chosen, the "pixels" will be stretched.

The binary is compiled for the STM32F024K6 microcontroller in the TSSOP20 package.

A video can be seen at https://youtu.be/kferDa9nGmw