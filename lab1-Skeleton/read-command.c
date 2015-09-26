// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

#include <stdio.h>

#define '&&': 0 //TODO: Forgot syntax

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

struct stack_node* pop(struct stack_node *head, int *operator, command_t *command)
{
    struct stack_node* temp = head;
		*operator = head->operator;
    *command = head->command;
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
bool isNewCommand(char curr, command* currCommand)
{

}

bool makeCommand(char curr, command* currCommand)
{

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
	command currCommand = NULL;

	while((curr = get_next_byte(get_next_byte_argument)) != EOF)
	{
		if(isNewCommand(curr, *currCommand))
		{
			//process mkcommand
			*currCommand = NULL;
			makeCommand(curr, *currCommand);
		}
		else
		{
			makeCommand(curr, *currCommand);
		}
	}

	//return some command_stream;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
