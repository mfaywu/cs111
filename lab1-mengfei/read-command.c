// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"
#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
static function definitions, etc. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define DEBUG false

struct command_stream
{
	command_t command;
	command_stream_t next;
} *stream, *tempStream, *headStream;

void initStream()
{
	stream = NULL;
	tempStream = NULL;
}

int numberOfOper(char *operator)
{
	switch (operator[0])
	{
	case ')': return 0;
	case '(': return 1;
	case ';': return 2;
	case '|':
		if (operator[1] == '|')
			return 3;
		else
			return 5;
	case '&': return 4;
	case '<': return 6; //Should never happen
	default: return 7; //Aka '>', should never happen
	}
	return -1; //Returns -1 if not an operator, then it's a command!
}

int precedence(int num) //TODO: Need to make sure these precedence cases are correct
{
	switch (num)
	{
	case 0:
		return 0;
	case 1:
		return 1;
	case 2:
		return 2;
	case 3:
	case 4:
		return 3;
	case 5:
		return 4;
	default:
		return 5; //aka '<' or '>', should never happen
	}
	return -1;
}

//***************************************//
//*** Stack for commands or operators ***//

struct stack_node
{
	int operator;
	command_t command;
	struct stack_node* next;
} *commandStack, *operatorStack, *temp, *head;

void initStacks()
{
	if (DEBUG) printf("Init stacks\n");
	commandStack = NULL;
	operatorStack = NULL;
	temp = NULL;
	head = NULL;
}

bool push(int operator, command_t command)
{
	if (DEBUG) printf("Push to stack%d\n", operator);
	if (operator == -1) //Getting a command
	{
		if (commandStack == NULL)
		{
			commandStack = (struct stack_node*)checked_malloc(sizeof(struct stack_node));
			commandStack->command = command;
			commandStack->next = NULL;
			if (DEBUG) printf("98 Push command to empty command stack \n");
			return true;
		}
		else
		{
			temp = (struct stack_node*)checked_malloc(sizeof(struct stack_node));
			temp->command = command;
			temp->next = commandStack;
			commandStack = temp;
			if (DEBUG) printf("108 Push command to nonempty stack \n");
			return true;
		}
	}
	else if (command == NULL) //Getting an operator
	{
		if (operatorStack == NULL)
		{
			operatorStack = (struct stack_node*)checked_malloc(sizeof(struct stack_node));
			operatorStack->operator = operator;
			operatorStack->next = NULL;
			if (DEBUG) printf("119Push operator to empty operatorStack %d\n", operator);
			return true;
		}
		else
		{
			temp = (struct stack_node*)checked_malloc(sizeof(struct stack_node));
			temp->operator = operator;
			temp->next = operatorStack;
			operatorStack = temp;
			if (DEBUG) printf("128 Push operator to nonempty stack %d\n", operator);
			return true;
		}
	}
	return false;
}

bool pop(bool isOperator)
{
	if (DEBUG) printf("137 Start to pop something\n");
	if (isOperator) //Pop off operator stack head
	{
		if (DEBUG) printf("140 Popping operator stack head\n");
		head = operatorStack;
		if (head == NULL)
			error(1, 0, "143 Popping off what doesn't exist");
		else
			head = operatorStack->next;
		operatorStack = head;
		return true;
	}
	else //pop off command stack head
	{
		if (DEBUG) printf("151 Popping command stack head\n");
		head = commandStack;
		if (head == NULL)
			error(1, 0, "154 Popping off what doesn't exist.");
		else
			head = commandStack->next;
		commandStack = head;
		return true;
	}
	return false;
}

command_t peekCommand()
{
	if (commandStack == NULL)
		return NULL;
	else
		return commandStack->command;
}

int peekOperator()
{
	if (operatorStack == NULL)
		return -300;
	else
		return operatorStack->operator;
}
//***************************************//

bool makeSimpleCommand(char** wordElement, char* inputElement, char* outputElement)
{
	command_t newCommand = (command_t)checked_malloc(sizeof(struct command));
	newCommand->type = SIMPLE_COMMAND;
	newCommand->u.word = checked_malloc(sizeof(char**));
	newCommand->u.word = wordElement;
	if (inputElement != NULL)
		newCommand->input = inputElement;
	if (outputElement != NULL)
		newCommand->output = outputElement;
	push(-1, newCommand);
	return true;
}

command_t makeSubshell(command_t topCommand)
{
	command_t newCommand = (command_t)checked_malloc(sizeof(struct command));
	newCommand->u.subshell_command = topCommand;
	newCommand->type = SUBSHELL_COMMAND;
	return newCommand;
}

command_t combine(command_t firstCommand, command_t secondCommand, int tempOper) //TODO: Doesn't handle subshell
{
	command_t newCommand = (command_t)checked_malloc(sizeof(struct command));
	newCommand->u.command[0] = firstCommand;
	newCommand->u.command[1] = secondCommand;
	//newCommand->status = -1;
	switch (tempOper)
	{
	case 2: // ;
		newCommand->type = SEQUENCE_COMMAND;
		return newCommand;
	case 3: // ||
		newCommand->type = OR_COMMAND;
		return newCommand;
	case 5: // |
		newCommand->type = PIPE_COMMAND;
		return newCommand;
	case 4: // &&
		newCommand->type = AND_COMMAND;
		return newCommand;
	default:
		return NULL;
	}
	return NULL;
}


void processOperator(int operator)
{
	if (operatorStack == NULL)
	{
		push(operator, NULL);
	}
	else
	{
		if (precedence(operator) > precedence(peekOperator()))
		{
			push(operator, NULL);
		}
		else
		{
			while (precedence(peekOperator()) != 0 && precedence(operator) <= precedence(peekOperator()))
			{
				int topOper = peekOperator();
				if(topOper == 1) 
				{
					command_t topCommand = peekCommand();
					pop(false);
					command_t newCommand = makeSubshell(topCommand);
				}
				if(topOper != 1)
				{
					int tempOper = peekOperator();
					pop(true);
					command_t secondCommand = peekCommand();
					pop(false);
					command_t firstCommand = peekCommand();
					pop(false);
					command_t newCommand = combine(firstCommand, secondCommand, tempOper);
					push(-1, newCommand);
				}
				if (operatorStack == NULL)
					break;
			}
			push(operator, NULL);
		}
	}
}

void completeCommand()
{
	while (operatorStack != NULL)
	{
		int tempOper = peekOperator();
		pop(true);
		command_t secondCommand = peekCommand();
		pop(false);
		command_t firstCommand = peekCommand();
		pop(false);
		command_t newCommand = combine(firstCommand, secondCommand, tempOper);
		push(-1, newCommand);
	}
	if (stream == NULL)
	{
		stream = (command_stream_t)checked_malloc(sizeof(struct command_stream));
		stream->command = commandStack->command;
		stream->next = NULL;
		headStream = (command_stream_t)checked_malloc(sizeof(struct command_stream));
		headStream = stream;
	}
	else
		if (stream != NULL)
		{
			tempStream = (command_stream_t)checked_malloc(sizeof(struct command_stream));
			tempStream->command = commandStack->command;
			stream->next = tempStream;
			stream = tempStream;
		}
	initStacks();
}

bool increaseSize(char* element, int used, int* bufferSize)
{
	*bufferSize += 10;
	element = (char*)checked_realloc(element, 10 * (*bufferSize));
	return true;
}

/* FIXME: Define the type 'struct command_stream' here. This should
complete the incomplete type declaration in command.h. */
command_stream_t make_command_stream(int(*get_next_byte) (void *),
	void *get_next_byte_argument)
{
	/* FIXME: Replace this with your implementation. You may need to
	add auxiliary functions and otherwise modify the source code.
	You can also use external functions defined in the GNU C Library. */
	if (DEBUG) printf("291 Begin make_command_stream\n");
	initStream();
	size_t sizeofBuffer = 1024;
	char* buffer = (char*)checked_malloc(sizeofBuffer);
	char curr;
	size_t filled = 0;
	while ((curr = get_next_byte(get_next_byte_argument)) != EOF) {
		buffer[filled] = curr;
		filled++;
		if (filled == sizeofBuffer) {
			sizeofBuffer *= 2;
			buffer = (char*)checked_grow_alloc(buffer, &sizeofBuffer);
		}
	}
	//***For the parser:
	//Word
	int bufferWordSize = 10;
	int currWord = 0;
	//char ** wordElement;
	char** wordElement = checked_malloc(sizeof(char*));
	wordElement[currWord] = (char*)checked_malloc(bufferWordSize);
	int usedWord = 0;
	//Input
	int bufferInputSize = 10;
	char* inputElement = (char*)checked_malloc(bufferInputSize);
	int usedInput = 0;
	//Output
	int bufferOutputSize = 10;
	char* outputElement = (char*)checked_malloc(bufferOutputSize);
	int usedOutput = 0;
	//***Initialize operator and command stacks
	initStacks();
	if (DEBUG) printf("333 Buffer created, initStacks()\n");
	
	int i = 0;
	while (i < (int)filled)
	{
		if (DEBUG) printf("on buffer %d\n", i);
		//***When you want to add a character*************************//
		int op = -1;
		int openCount = 0;
		int closedCount = 0;

		if (buffer[i] == '`')
			error(1, 0, "Line %d: %c", __LINE__, buffer[i]);

		if (buffer[i] == '(')
		{
			openCount = 1;
			closedCount = 0;
			int x = i;
			while (x < (int) filled)
			{
				x++;
				if (buffer[x] == '(')
					openCount++;
				if (buffer[x] == ')')
					closedCount++;
				if (closedCount == openCount)
					break;
			}
			if (closedCount != openCount)
				error(1, 0, "Line %d: Expected ')' for end of subshell", __LINE__);
		}
		if(buffer[i] == ')')
		{
			if(openCount == 0)
				error(1, 0, "Line %d: Expected '(' before ')'", __LINE__);
		}

		if(buffer[i] = '(') 
		{
			op = numberOfOper(&buffer[i]);
			processOperator(op);
		}

		//Case of ' '
		while (buffer[i] == ' ' && usedWord == 0)
			i++;

		if (buffer[i] == ' ' && usedWord != 0)
		{
			if (usedWord >= bufferWordSize)
			{
				bufferWordSize += 10;
				wordElement[currWord] = (char*)checked_realloc(wordElement[currWord], bufferWordSize);
			}
			//i++;
			wordElement[currWord][usedWord] = '\0';
			while (buffer[i + 1] == ' ')
				i++;
			usedWord = 0;
			bufferWordSize = 10;
			currWord++;
			wordElement[currWord] = (char*)checked_malloc(bufferWordSize);
			//wordElement[currWord][usedWord] = buffer[i];
			//usedWord++;
		}
		//Case of carrots
		//WHAT IF a<b>c>d<a....?! You're making a simple command out of a<b>c which then
		// must also be the word of >d<a
		//Case of '<' first
		else
			if (buffer[i] == '<')
			{
				int k = i;
				while (buffer[k-1] == ' ')
				{
					k--;
					if(buffer[k-1] == '\n')
						error(1, 0, "Line %d: Operator and word on diff lines", __LINE__);
				}
				if (wordElement[0][0] == '\0')
					error(1, 0, "Line %d: No command given to '<'", __LINE__);
				i++;
				while (buffer[i] != '<' && buffer[i] != '>' && buffer[i] != '&' && buffer[i] != '|' && buffer[i] != '(' && buffer[i] != ')' && buffer[i] != ';' && buffer[i] != '\n')
				{
					if (i == filled)
					{
						if (DEBUG) printf("378 Complete command, free buffer bc EOF\n");
						currWord++;
						wordElement[currWord] = '\0';
						if (usedInput == 0)
							inputElement = NULL;
						if (usedOutput == 0)
							outputElement = NULL;
						makeSimpleCommand(wordElement, inputElement, outputElement);
						completeCommand();
						//free(buffer);
						return headStream;
					}
					if (buffer[i] == ' ')
						i++;
					else
					{
						if (usedInput >= bufferInputSize)
						{
							bufferInputSize += 10;
							inputElement = (char*)checked_realloc(inputElement, bufferInputSize);
						}
						inputElement[usedInput] = buffer[i];
						usedInput++;
						i++;
					}
				}
				if (usedInput >= bufferInputSize)
				{
					bufferInputSize += 10;
					inputElement = (char*)checked_realloc(inputElement, bufferInputSize);
				}
				if (usedInput == 0)
					error(1, 0, "Line %d: No input after '<'", __LINE__);
				inputElement[usedInput] = '\0';
				usedInput++;
				if (buffer[i] == '>')
				{
					i++;
					while (buffer[i] != '<' && buffer[i] != '>' && buffer[i] != '&' && buffer[i] != '|' && buffer[i] != '(' && buffer[i] != ')' && buffer[i] != ';' && buffer[i] != '\n')
					{
						if (i == filled)
						{
							if (DEBUG) printf("413 Complete command, free buffer at EOF\n");
							currWord++;
							wordElement[currWord] = '\0';
							if (usedInput == 0)
								inputElement = NULL;
							if (usedOutput == 0)
								outputElement = NULL;
							makeSimpleCommand(wordElement, inputElement, outputElement);
							completeCommand();
							//free(buffer);
							return headStream;
						}
						if (buffer[i] == ' ')
							i++;
						else
						{
							if (usedOutput >= bufferOutputSize)
							{
								bufferOutputSize += 10;
								outputElement = (char*)checked_realloc(outputElement, bufferOutputSize);
							}
							outputElement[usedOutput] = buffer[i];
							usedOutput++;
							i++;
						}
					}
					if (usedOutput >= bufferOutputSize)
					{
						bufferOutputSize += 10;
						outputElement = (char*)checked_realloc(outputElement, bufferOutputSize);
					}
					if (usedOutput == 0)
						error(1, 0, "Line %d: No input after '<'", __LINE__);
					outputElement[usedOutput] = '\0';
					usedOutput++;
					//i--; //Check logic
				}
				i--;
			}
		/////CHECK FOR EXTRA i++!!!!!
		//Case of '>' first
			else
				if (buffer[i] == '>')
				{
					int k = i;
					while (buffer[k-1] == ' ')
					{
						k--;
						if(buffer[k-1] == '\n')
							error(1, 0, "Line %d: Operator and word on diff lines", __LINE__);
					}
					if (wordElement[0][0] == '\0')
					error(1, 0, "Line %d: No command given to '<'", __LINE__);
					i++;
					while (buffer[i] != '<' && buffer[i] != '>' && buffer[i] != '&' && buffer[i] != '|' && buffer[i] != '(' && buffer[i] != ')' && buffer[i] != ';' && buffer[i] != '\n')
					{
						if (i == filled)
						{
							if (DEBUG) printf("471 Complete Command, free buffer at EOF");
							currWord++;
							wordElement[currWord] = '\0';
							if (usedInput == 0)
								inputElement = NULL;
							if (usedOutput == 0)
								outputElement = NULL;
							makeSimpleCommand(wordElement, inputElement, outputElement);
							completeCommand();
							//free(buffer);
							return headStream;
						}
						if (buffer[i] == ' ')
							i++;
						else
						{
							if (usedOutput >= bufferOutputSize)
							{
								bufferOutputSize += 10;
								outputElement = (char*)checked_realloc(outputElement, bufferOutputSize);
							}
							outputElement[usedOutput] = buffer[i];
							usedOutput++;
							i++;
						}
					}
					if (usedOutput >= bufferOutputSize)
					{
						bufferOutputSize += 10;
						outputElement = (char*)checked_realloc(outputElement, bufferOutputSize);
					}
					if (usedOutput == 0)
						error(1, 0, "Line %d: No input after '<'", __LINE__);
					outputElement[usedOutput] = '\0';
					usedOutput++;
					if (buffer[i] == '<')
					{
						i++;
						while (buffer[i] != '<' && buffer[i] != '>' && buffer[i] != '&' && buffer[i] != '|' && buffer[i] != '(' && buffer[i] != ')' && buffer[i] != ';' && buffer[i] != '\n')
						{
							if (i == filled)
							{
								if (DEBUG) printf("505 Complete Command, free buffer at EOF");
								currWord++;
								wordElement[currWord] = '\0';
								if (usedInput == 0)
									inputElement = NULL;
								if (usedOutput == 0)
									outputElement = NULL;
								makeSimpleCommand(wordElement, inputElement, outputElement);
								completeCommand();
								//free(buffer);
								return headStream;
							}
							if (buffer[i] == ' ')
								i++;
							else
							{
								if (usedInput >= bufferInputSize)
								{
									bufferInputSize += 10;
									inputElement = (char*)checked_realloc(inputElement, bufferInputSize);
								}
								inputElement[usedInput] = buffer[i];
								usedInput++;
								i++;
							}
						}
						if (usedInput >= bufferInputSize)
						{
							bufferInputSize += 10;
							inputElement = (char*)checked_realloc(inputElement, bufferInputSize);
						}
						if (usedInput == 0)
							error(1, 0, "Line %d: No input after '<'", __LINE__);
						inputElement[usedInput] = '\0';
						usedInput++;
					}
					wordElement[currWord + 1] = '\0';
					/*if (usedInput == 0)
					inputElement = NULL;
					if (usedOutput == 0)
					outputElement = NULL;
					if(DEBUG) printf("makeSimpleCommand %s\n", wordElement[0]);
					makeSimpleCommand(wordElement, inputElement, outputElement);
					bufferWordSize = 10;
					currWord = 0;
					usedWord = 0;
					usedInput = 0;
					usedOutput = 0;
					wordElement = (char**)checked_malloc(sizeof(char*));
					wordElement[currWord] = (char*)checked_malloc(bufferWordSize);
					inputElement = (char*)checked_malloc(bufferInputSize);
					outputElement = (char*)checked_malloc(bufferOutputSize);*/
					i--;
				}
		//Operators
		//After every operator is encountered, use makeSimpleCommand to push the command on the stack
		//Case of '|'
				else
					if (buffer[i] == '|')
					{
						int k = i;
						while (buffer[k-1] == ' ')
						{
							k--;
							if(buffer[k-1] == '\n')
								error(1, 0, "Line %d: Operator and word on diff lines", __LINE__);
						}
						
						if (buffer[i+1] == '|' && operatorStack == NULL)
							error(1, 0, "Line %d: Missing pipe for '||'", __LINE__);
						if (wordElement[0][0] == '\0')
							error(1, 0, "Line %d: Nothing for '|'", __LINE__);
						//if (commandStack == NULL)
						//	error(1, 0, "Line %d: Nothing to run '|' on");
						if (buffer[i + 1] == '|' && buffer[i+2] == '|')
							error(1, 0, "Line %d: Invalid Command, too many '|'", i);
						op = numberOfOper(&buffer[i]);
						//error(1, 0, "Line %d: Nothing to pipe", __LINE__);
						if (buffer[i-1] != ' ')
							currWord++;
						wordElement[currWord] = '\0';
						if (usedInput == 0)
							inputElement = NULL;
						if (usedOutput == 0)
							outputElement = NULL;
						if (DEBUG) printf(" 566 makeSimpleCommand %s\n", wordElement[0]);
						makeSimpleCommand(wordElement, inputElement, outputElement);
						bufferWordSize = 10;
						currWord = 0;
						usedWord = 0;
						usedInput = 0;
						usedOutput = 0;
						wordElement = (char**)checked_malloc(sizeof(char*));
						wordElement[currWord] = (char*)checked_malloc(bufferWordSize);
						inputElement = (char*)checked_malloc(bufferInputSize);
						outputElement = (char*)checked_malloc(bufferOutputSize);
						if (DEBUG) printf("577 Process operator %d\n", op);
						processOperator(op);
						if (op == 3) 
							i++;
						if (buffer[i + 1] == ' ')
							i++;
						//i++;
					}
		//Case of '&'
					else
						if (buffer[i] == '&')
						{
							int k = i;
							while (buffer[k-1] == ' ')
							{
								k--;
								if(buffer[k-1] == '\n')
									error(1, 0, "Line %d: Operator and word on diff lines", __LINE__);
							}
							//if (buffer[i] == '&' && operatorStack == NULL)
							//	error(1, 0, "Line %d: Missing pipe for '&&'", __LINE__);
							if (wordElement[0][0] == '\0')
								error(1, 0, "Line %d: Nothing for '|'", __LINE__);
							if (buffer[i + 1] == '&' && buffer[i+2] == '&')
								error(1, 0, "Line %d: Invalid Command, too many '&'", i);
							op = numberOfOper(&buffer[i]);
							if (buffer[i-1] != ' ')
								currWord++;
							wordElement[currWord] = '\0';
							if (usedInput == 0)
								inputElement = NULL;
							if (usedOutput == 0)
								outputElement = NULL;
							if (DEBUG) printf("592 makeSimpleCommand %s\n", wordElement[0]);
							makeSimpleCommand(wordElement, inputElement, outputElement);
							bufferWordSize = 10;
							usedWord = 0;
							currWord = 0;
							usedInput = 0;
							usedOutput = 0;
							wordElement = (char**)checked_malloc(sizeof(char*));
							wordElement[currWord] = (char*)checked_malloc(bufferWordSize);
							inputElement = (char*)checked_malloc(bufferInputSize);
							outputElement = (char*)checked_malloc(bufferOutputSize);
							processOperator(op);
							i++;
							if (buffer[i + 1] == ' ')
								i++;
						}
		//Case of ';'
						else
							if (buffer[i] == ';')
							{
													int k = i;
						while (buffer[k-1] == ' ')
						{
							k--;
							if(buffer[k-1] == '\n')
								error(1, 0, "Line %d: Operator and word on diff lines", __LINE__);
						}
								if(wordElement[0][0] == '\0')
									error(1, 0, "Line %d: Nothing before sequence", i);
								op = numberOfOper(&buffer[i]);
								currWord++;
								wordElement[currWord] = '\0';
								if (usedInput == 0)
									inputElement = NULL;
								if (usedOutput == 0)
									outputElement = NULL;
								if (DEBUG) printf("617 makeSimpleCommand %s\n", wordElement[0]);
								if ((currWord = 0) || (currWord == 1))
									error(1, 0, "Line %d: Nothing to run ';' on", i);
								makeSimpleCommand(wordElement, inputElement, outputElement);
								bufferWordSize = 10;
								usedWord = 0;
								currWord = 0;
								usedInput = 0;
								usedOutput = 0;
								wordElement = (char**)checked_malloc(sizeof(char*));
								wordElement[currWord] = (char*)checked_malloc(bufferWordSize);
								inputElement = (char*)checked_malloc(bufferInputSize);
								outputElement = (char*)checked_malloc(bufferOutputSize);
								processOperator(op);
								//i++;
							}
		//Case of '\n'
							else
								if (buffer[i] == '\n' && i != 0)
								{
									/*int scChecker = i;
									while (buffer[i + 1] == ' ')
									{
										scChecker++;
										if (buffer[scChecker + 1] == ';')
											error(1, 0, "Line %d: Newline followed by ';'");
									}*/
									if (buffer[i+1] == ';')
										error(1, 0, "Line %d: Newline followed by ';'", i);
									if ((i + 1) == (int)filled)
									{
										currWord++;
										wordElement[currWord] = '\0';
										if (usedInput == 0)
											inputElement = NULL;
										if (usedOutput == 0)
											outputElement = NULL;
										if (DEBUG) printf("654 makeSimpleCommand %s\n", wordElement[0]);
										makeSimpleCommand(wordElement, inputElement, outputElement);
										bufferWordSize = 10;
										currWord = 0;
										usedWord = 0;
										usedInput = 0;
										usedOutput = 0;
										wordElement = (char**)checked_malloc(sizeof(char*));
										wordElement[currWord] = (char*)checked_malloc(bufferWordSize);
										inputElement = (char*)checked_malloc(bufferInputSize);
										outputElement = (char*)checked_malloc(bufferOutputSize);
										completeCommand();
										free(buffer);
										return headStream;
									}
									char lastC;
									int back = 1;
									while (buffer[i - back] == ' ' && i - back >= 0)
									{
										//lastC = buffer[i - back];
										back++;
									}
									lastC = buffer[i - back];
									if (buffer[i + 1] == '\n')
									{
										while (buffer[i + 1] == '\n')
											i++;
										if (lastC != '|' && lastC != '&')
										{
											currWord++;
											wordElement[currWord] = '\0';
											if (usedInput == 0)
												inputElement = NULL;
											if (usedOutput == 0)
												outputElement = NULL;
											if (DEBUG) printf("654 makeSimpleCommand %s\n", wordElement[0]);
											makeSimpleCommand(wordElement, inputElement, outputElement);
											bufferWordSize = 10;
											currWord = 0;
											usedWord = 0;
											usedInput = 0;
											usedOutput = 0;
											wordElement = (char**)checked_malloc(sizeof(char*));
											wordElement[currWord] = (char*)checked_malloc(bufferWordSize);
											inputElement = (char*)checked_malloc(bufferInputSize);
											outputElement = (char*)checked_malloc(bufferOutputSize);
											completeCommand();
										}
									}
									else
									{
										if (lastC == '|' || lastC == '&' || lastC == '<' || lastC || '>')
										{
											while (buffer[i + 1] == '\n')
												i++;//////SPIT ERROR?
										}
										else
										{
											char swap = ';';
											op = numberOfOper(&swap);
											wordElement[currWord + 1] = '\0';
											if (usedInput == 0)
												inputElement = NULL;
											if (usedOutput == 0)
												outputElement = NULL;
											if (DEBUG) printf(" 679 makeSimpleCommand %s\n", wordElement[0]);
											makeSimpleCommand(wordElement, inputElement, outputElement);
											bufferWordSize = 10;
											currWord = 0;
											usedWord = 0;
											usedInput = 0;
											usedOutput = 0;
											wordElement = (char**)checked_malloc(sizeof(char*));
											wordElement[currWord] = (char*)checked_malloc(bufferWordSize);
											inputElement = (char*)checked_malloc(bufferInputSize);
											outputElement = (char*)checked_malloc(bufferOutputSize);
											processOperator(op);
										}
									}
								}
		//Case of # (comment)
								else
									if (buffer[i] == '#')
									{
										if (DEBUG) printf("698 Got a comment!\n");
										while (buffer[i] != '\n')
											i++;
									}
									//Else
									else
									{
										if (buffer[i] == '\'')
										{
											if (buffer[i + 1] == 'E' && buffer[i + 2] == 'O' && buffer[i + 3] == 'F')
												break;
										}
										//if (buffer[i] != ' ')
										wordElement[currWord][usedWord] = buffer[i];
										usedWord++;
										if (i + 1 == filled)
										{
											currWord++;
											wordElement[currWord] = '\0';
											if (usedInput == 0)
												inputElement = NULL;
											if (usedOutput == 0)
												outputElement = NULL;
											makeSimpleCommand(wordElement, inputElement, outputElement);
											completeCommand();
										}
									}
		i++;
	}
	free(buffer);
	return headStream;
}
command_t
read_command_stream(command_stream_t s)
{
	/* FIXME: Replace this with your implementation too. */
	//error (1, 0, "command reading not yet implemented");
	//return 0;
	if (headStream != NULL) {
		//if (s->command)
		//{
		// command_t cmd = s->command;
		// printf("s address: %p\n", &s);
		// printf("command word: %s\n", s->command->u.word[0]);
		// printf("s word address pre next: %p\n", &(s->command->u.word[0]));

		// command_stream_t temp = s;
		// printf("s->next address: %p\n", &(s->next));
		// //s = (command_stream_t) checked_malloc(sizeof(struct command_stream));
		// s = (s->next);
		// printf("s address after next: %p\n", &s);
		// printf("s word address post next: %p\n", &(s->command->u.word[0]));

		// printf("command word after next: %s\n", s->command->u.word[0]);
		// free(temp); 
		// printf("command word after free: %s\n", s->command->u.word[0]);

		command_t cmd = headStream->command;
		command_stream_t temp = headStream;
		headStream = headStream->next;
		free(temp);
		return cmd;
		//}
	}
	return NULL;
}
// Change Notes
// Changed all "\0" to '\0'
// STREAM ONLY PRINTS LAST COMMAND, IN REVERSE ORDER???!!!!!
// Pretty sure we only need to fix 7 and 8 now
// FOR SOME REASON THERE IS AN OR COMMAND LEFT ON Stream PRIOR TO NUMBER 7 ---> i ~= 168
//  (STREAM MAYBE NOT REALLOCATED?)
// wordElement is 1d array in 6

//10/19/15
//Fixed carrots running past (see line 405, check if i == filled)
//Line ~699, added makeSimpleCommand
//Commented out all free(buffer), which fixed memory errors when order of commands switched around in test script (don't know why free worked sometimes, failed others)
//Added makeSimpleCommand before all calls to completeCommand
