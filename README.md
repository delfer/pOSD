Perfectum OSD

Stage 1

Video signal detector
using 3 Resistors and ADC Watchdog

Connections:

3V3 ---|
      | |
      | | R1=560 OMh
      | |
       |
PA0 ---+
       |
      | |
      | | R2=75 OMh
      | |
       |
       +---------------> Camera Video
       |
      | |
      | | R3=75 OMh
      | |
       |
GND ---+---------------> Camera GND

Output to USART1 at 115200
After detecting write PAL/NTSC and lines count

You can try different R1 and R2 combinations. Algorithm can detect right video sync level.
 