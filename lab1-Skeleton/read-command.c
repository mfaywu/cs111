// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

#include <stdio.h>

int numberOfOper(cstring * operator)
{
	switch(operator)
	{
		case '(': return 0;
		case ')': return 1;
		case ';': return 2;
		case '|': return 3;
		case '&&': return 4;
		case '||': return 5;
		case '<': return 6;
		default: return 7; //Aka '>'
	}
	return -1; //Returns -1 if not an operator, then it's a command!
}

int precedence(int num) //TODO: Need to make all precedence cases
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
        exit(0);
    }
		temp->operator = operator;
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

//TODO: Returns true if curr is separate from the currCommand
//false if curr should be concatenated to currCommand
bool isNewElement(char curr, cstring* currElement)
{

}

bool makeElement(char curr, cstring* currElement)
{

}

void process(cstring *currElement, struct stack_node *operatorStack, struct stack_node *commandStack)
{
	int opNum = numberOfOper(*currElement);

	if(opNum == -1)
	{
		push(*commandStack, NULL, *currElement);
	}
	else
	{
		if(*operatorstack == NULL)
		{
			push(*operatorStack, *currElement, NULL);
		}
		else
		{
			if(precedence(opNum)) > precedence(peekOperator(*operatorStack)))
			{
				push(*operatorStack, *currElement, NULL);
			}
			else
			{
				while(peekOperator(*operatorStack) != 0 && precedence(opNum) <= precedence(peekOperator(*operatorStack))) //TODO: #define '(' as 0
				{
					int tempOper = peekOperator(*operatorStack);
					pop(*operatorStack);
					int secondCommand = peekCommand(*commandStack);
					pop(*commandStack);
					int firstCommand = peekCommand(*commandStack);
					pop(*commandStack);
					command newCommand = combine(firstCommand, secondCommand, tempOper); //TODO: Write combine
					push(*commandStack, NULL, newCommand);
					if(*operatorStack == NULL)
					{
						break;
					}
				}
			}
			push(*operatorStack, *currElement, NULL);
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

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  //error (1, 0, "command reading not yet implemented");
  //return 0;

	char curr;
	cstring currElement = NULL;

	struct stack_node* operatorStack;
	struct stack_node* commandStack;

	while((curr = get_next_byte(get_next_byte_argument)) != EOF)
	{
		if(!isNewElement(curr, *currElement))
		{
			process(*currElement, *operatorStack, *commandStack); //TODO: How do you know the stream is done?
			*currElement = NULL;
			makeElement(curr, *currElement);
		}
		else
		{
			makeElement(curr, *currElement);
		}
	}

	//return some pointer to a linkedlist of commands aka command_stream;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
