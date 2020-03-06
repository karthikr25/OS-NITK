char* frameBuffer = (char*) 0x000B8000;
int numLocations = 2000;
int lineLength = 80;
int numLines = 25;

int curLocation = 0;

int firstInput = 1;
int shiftKey = 0;

/*
Black  0	Red			4	 Dark grey		8	  Light red	     12
Blue   1	Magenta		5	 Light blue		9	  Light magenta	 13
Green  2	Brown		6	 Light green	10	  Light brown	 14
Cyan   3	Light grey	7	 Light cyan		11	  White	         15
*/

int scanCodeToASCII_woShift_arr[] = {-1, -1, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', -1, -1, 'q', 'w', 'e', 'r', 't', 'y',
									'u', 'i', 'o', 'p', '[', ']', -2, -1, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', -3, '\\',
									'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', -3, -1, -1, ' ', -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
									-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    								};
int scanCodeToASCII_wShift_arr[] = {-1, -1, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', -1, -1, 'Q', 'W', 'E', 'R', 'T', 'Y',
									'U', 'I', 'O', 'P', '{', '}', -2, -1, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', -3, '|',
									'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', -3, -1, -1, ' ', -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
									-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    								};

extern void outb(unsigned short port, unsigned char data);
extern unsigned char inb(unsigned short port);

void writeCharToFB_Loc(char ch, unsigned char foreground, unsigned char background, int location);
void writeCharToFB(char ch, unsigned char foreground, unsigned char background);
void writeStringToFB(char* str, unsigned char foreground, unsigned char background);

void endOfBufferBehaviour();
void movetoNextLine();
void clearScreen();

void enableCursor();
void disableCursor();
void moveCursor(int location);

void swap(char* a, char* b);
void reverse(char* str, int length);
char* itoa(int num, char* str, int base);

char scanCodeToASCII_woShift(int input);
int takeInput(char* str, int maxLen);

//MAIN FUNCTION
void mainFunc()
{
	clearScreen();
	enableCursor();
	moveCursor(0);

	writeStringToFB("Testing\n", 7, 0);

	char str[100];
	takeInput(str, 100);
	writeStringToFB(str, 0, 7);

	writeStringToFB("\nDone\n", 7, 0);

	takeInput(str, 100);
	writeStringToFB(str, 0, 7);

	writeStringToFB("\nDone\n", 7, 0);

	takeInput(str, 100);
	writeStringToFB(str, 0, 7);

	writeStringToFB("\nDone\n", 7, 0);
}

void writeCharToFB_Loc(char ch, unsigned char foreground, unsigned char background, int location)
{
	frameBuffer[2*location] = ch;
	frameBuffer[2*location+1] = ((background & 0xF) << 4) | (foreground & 0xF);
}

void writeCharToFB(char ch, unsigned char foreground, unsigned char background)
{
	if (ch == '\n')
	{
		movetoNextLine();
		endOfBufferBehaviour();
		return;
	}

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

int scanCodeToASCII(int input)
{
	if (input >= 83)
	{
		return -1;
	}

	if (shiftKey)
	{
		return scanCodeToASCII_wShift_arr[input];
	}
	else
	{
		return scanCodeToASCII_woShift_arr[input];
	}
}

int takeInput(char* str, int maxLen)
{
	int count = 0;
	unsigned char input = 0;

	int firstEnter = 1;
	int firstPos = curLocation;
	
	while (count < (maxLen-1))
	{
		if (inb(0x60) != input)
		{
			input = inb(0x60);

			if (input == 28)
			{
				if (firstInput)
				{
					break;
				}
				else
				{
					if (firstEnter)
					{
						firstEnter = 0;
						continue;
					}
					else
					{
						break;
					}
				}
			}

			if (input == 14)
			{
				if (curLocation == firstPos)
				{
					continue;
				}
				else
				{
					curLocation--;
					if (curLocation < 0)
					{
						curLocation = 0;
					}
					writeCharToFB_Loc(' ', 7, 0, curLocation);
					moveCursor(curLocation);
					count--;
					continue;
				}
			}

			if ((int) input == 0x2A || (int) input == 0x36)
			{
				shiftKey = 1;
				continue;
			}

			if ((int) input == 0xAA || (int) input == 0xB6)
			{
				shiftKey = 0;
				continue;
			}

			if (input > 0)
			{
				if (scanCodeToASCII((int) input) >= 0)
				{
					str[count++] = scanCodeToASCII((int) input);
					writeCharToFB(scanCodeToASCII((int) input), 7, 0);
					moveCursor(curLocation);
				}
			}
		}
	}
	str[count] = '\0';
	writeCharToFB('\n', 7, 0);
	moveCursor(curLocation);

	if (firstInput)
	{
		firstInput = 0;
	}

	return count;
}