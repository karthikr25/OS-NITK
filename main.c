char* frameBuffer = (char*) 0x000B8000;
int numLocations = 2000;
int lineLength = 80;
int numLines = 25;

int curLocation = 0;

/*
Black  0	Red			4	 Dark grey		8	  Light red	     12
Blue   1	Magenta		5	 Light blue		9	  Light magenta	 13
Green  2	Brown		6	 Light green	10	  Light brown	 14
Cyan   3	Light grey	7	 Light cyan		11	  White	         15
*/
void writeCharToFB_Loc(char ch, unsigned char foreground, unsigned char background, int location);
void writeCharToFB(char ch, unsigned char foreground, unsigned char background);
void writeStringToFB(char* str, unsigned char foreground, unsigned char background);

void endOfBufferBehaviour();
void movetoNextLine();
void clearScreen();

extern void outb(unsigned short port, unsigned char data);
extern unsigned char inb(unsigned short port);

void enableCursor();
void disableCursor();
void moveCursor(int location);

void swap(char* a, char* b);
void reverse(char* str, int length);
char* itoa(int num, char* str, int base);

void serial_configure_baud_rate(unsigned short com, unsigned short divisor);
void serial_configure_line(unsigned short com);
int serial_is_transmit_fifo_empty(unsigned int com);

//MAIN FUNCTION
void mainFunc()
{
	clearScreen();
	enableCursor();
	moveCursor(0);

	writeStringToFB("Testing", 7, 0);
}

void writeCharToFB_Loc(char ch, unsigned char foreground, unsigned char background, int location)
{
	frameBuffer[2*location] = ch;
    frameBuffer[2*location+1] = ((background & 0xF) << 4) | (foreground & 0xF);
}

void writeCharToFB(char ch, unsigned char foreground, unsigned char background)
{
	frameBuffer[2*curLocation] = ch;
    frameBuffer[2*curLocation+1] = ((background & 0xF) << 4) | (foreground & 0xF);
    curLocation++;

    endOfBufferBehaviour();
}

void writeStringToFB(char* str, unsigned char foreground, unsigned char background)
{
	while (*str)
	{
		if (*str == '\n')
		{
			movetoNextLine();
			str++;
			continue;
		}

		writeCharToFB(*str, foreground, background);
		str++;
	}

	moveCursor(curLocation);
}

void endOfBufferBehaviour()
{
	if (curLocation >= numLocations)
    {
    	for (int a = 0; a < (numLocations-lineLength); a++)
    	{
    		frameBuffer[2*a] = frameBuffer[2*(a+lineLength)];
    		frameBuffer[2*a+1] = frameBuffer[2*(a+lineLength)+1];
    	}

    	for (int a = (numLocations-lineLength); a < numLocations; a++)
    	{
    		writeCharToFB_Loc(' ', 0, 0, a);
    	}

    	curLocation = (numLocations-lineLength);
    }
}

void movetoNextLine()
{
	curLocation += (lineLength - (curLocation % lineLength));

	endOfBufferBehaviour();
}

void clearScreen()
{
	for (int a = 0; a < numLocations; a++)
    {
    	writeCharToFB_Loc(' ', 7, 0, a);
    }
}

void enableCursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | 15);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

void disableCursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void moveCursor(int location)
{
	outb(0x3D4, 0x0F);
	outb(0x3D5, (location & 0xFF));

	outb(0x3D4, 0x0E);
	outb(0x3D5, ((location >> 8) & 0xFF));
}

void swap(char* a, char* b)
{
	char temp = *a;
	*a = *b;
	*b = temp;
}

void reverse(char* str, int length) 
{ 
    int start = 0; 
    int end = length -1; 
    while (start < end) 
    { 
        swap((str+start), (str+end)); 
        start++; 
        end--; 
    } 
} 

char* itoa(int num, char* str, int base) 
{ 
    int i = 0; 
    int isNegative = 0; 
  
    if (num == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
  
    if (num < 0 && base == 10) 
    { 
        isNegative = 1; 
        num *= -1; 
    } 
  
    while (num != 0) 
    { 
        int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num /= base; 
    } 

    if (isNegative)
    {
        str[i++] = '-'; 
    }
  
    str[i] = '\0';
    reverse(str, i); 
  
    return str; 
} 

# define SERIAL_COM1_BASE  0x3F8      //COM1 base port 
/*SERIAL_DATA_PORT(base)          (base)
  SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
  SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
  SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
  SERIAL_LINE_STATUS_PORT(base)   (base + 5)
*/
void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
    outb(com+3,0x80);   //Tells the serial port to expect first the highest 8 bits on the data port, then the lowest 8 bits will follow
    outb(com,(divisor >> 8) & 0x00FF);
    outb(com,divisor & 0x00FF);
}

void serial_configure_line(unsigned short com)
{
	/* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |       d	Enables (d = 1) or disables (d = 0) DLAB , b	If break control enabled (b = 1) disabled (b = 0)
                                                  prty	The number of parity bits to use, s	The number of stop bits to use (s = 0 equals 1, s = 1 equals 1.5 or 2)
                                                  dl	Describes the length of the data
     * Content: | d | b | prty  | s | dl  |
     * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
    */
	outb(com+3, 0x03);
	outb(com+3, 0xC7);
	outb(com+4, 0x03);  
}

int serial_is_transmit_fifo_empty(unsigned int com)
{
    /* 0x20 = 0010 0000 5th bit is 1 so the buffer is empty*/
    return inb(com+5) & 0x20;
}