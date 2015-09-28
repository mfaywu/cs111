// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int numberOfOper(char *operator)
{
	switch(operator[0])
	{
		case '(': return 0;
		case ')': return 1;
		case ';': return 2;
		case '|': 
			if(operator[1] == '|')
				return 3;
			else
				return 5;
		case '&': return 4;
		case '<': return 6;
		default: return 7; //Aka '>'
	}
	return -1; //Returns -1 if not an operator, then it's a command!
}

int precedence(int num) //TODO: Need to make sure these precedence cases are correct
{
	switch(num)
	{
		case 0:
		case 1:
		 return 0;
		case 2:
		case 3:
			return 1;
		case 4:
		case 5:
			return 2;
		default:
			return 3; //aka '<' or '>'
	}
	return -1;
}

//***************************************//
//*** Stack for commands or operators ***//

struct stack_node
{
	int operator;
	command_t command;
	struct stack_node *next;
}

void init(struct stack_node* head)
{
    head = NULL;
}

struct stack_node* push(struct stack_node* head, int operator, command_t command)
{
    struct stack_node* temp = (struct stack_node*)malloc(sizeof(struct stack_node));
    if(temp == NULL)
    {
        error(1, 0, "No space left.");
    }
	if(operator != -1)
		temp->operator = operator;
	if(command != NULL)
		temp->command = command;
    temp->next = head;
    head = temp;
    return head;
}

struct stack_node* pop(struct stack_node *head)
{
    struct stack_node* temp = head;
    head = head->next;
    free(temp);
    return head;
}

command_t peekCommand(struct stack_node* head)
{
	return head->command;
}

int peekOperator(struct stack_node* head)
{
	return head->operator;
}

//***************************************//

//TODO: Returns true if curr is separate from the currCommand
//false if curr should be concatenated to currCommand
//isNewCommand returns true if this is the end of the complete command
bool isNewElement(char curr, char *currElement, bool *isNewCommand)
{
	return false; //TODO
}

bool makeElement(char curr, char *currElement)
{
	return false; //TODO
}

command_t makeCommand(char *currElement) 
{
	return NULL; //TODO
}

command_t combine (command_t firstCommand, command_t secondCommand, int temOper) //TODO
{
	return NULL;
}

void process(char *currElement, struct stack_node *operatorStack, struct stack_node *commandStack)
{
	int opNum = numberOfOper(currElement);

	if(opNum == -1)
	{
		command_t tempCommand = makeCommand(currElement);
		push(commandStack, -1, tempCommand);
	}
	else
	{
		if(operatorStack == NULL)
		{
			push(operatorStack, *currElement, NULL);
		}
		else
		{
			if(precedence(opNum) > precedence(peekOperator(operatorStack)))
			{
				push(operatorStack, *currElement, NULL);
			}
			else
			{
				while(precedence(peekOperator(operatorStack)) != 0 && precedence(opNum) <= precedence(peekOperator(operatorStack))) //TODO: #define '(' as 0
				{
					int tempOper = peekOperator(operatorStack);
					pop(operatorStack);
					command_t secondCommand = peekCommand(commandStack);
					pop(commandStack);
					command_t firstCommand = peekCommand(commandStack);
					pop(commandStack);
					command_t newCommand = combine(firstCommand, secondCommand, tempOper); //TODO: Write combine
					push(commandStack, -1, newCommand);
					if(operatorStack == NULL)
					{
						break;
					}
				}
			}
			push(operatorStack, *currElement, NULL);
		}
	}
}

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

struct command_stream //TODO
{
	command_t *command;
	struct command_stream *next;
}

command_stream_t make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

	char curr;
	char *currElement = NULL;
	bool *isNewCommand;
	//have a pointer to the first complete command, which we'll call stream

	struct stack_node* operatorStack;
	struct stack_node* commandStack;

	while((curr = get_next_byte(get_next_byte_argument)) != EOF)
	{
		if(isNewElement(curr, *currElement, *isNewCommand))
		{
			process(*currElement, *operatorStack, *commandStack); //TODO: How do you know the stream is done?
			*currElement = NULL;
			makeElement(curr, *currElement);
			if(*isNewCommand)
			{
				//that means this complete command is over, so store this into the command_stream as a new node
			}
		}
		else
		{
			makeElement(curr, *currElement);
		}
	}
	return NULL;
	//return stream, which is some pointer to a linkedlist of commands aka command_stream;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
