#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iks_iloc.h"
#include "iks_ast.h"
#include "iks_tree.h"

iks_list_t *label_code_generator(char *l) {
	iks_list_t *code;
	code = new_iks_list();
	iks_list_set_item(code,l);
	return code;
}

void code_or(iks_tree_t **ast) {
	iks_ast_node_value_t *or_n = (*ast)->item;
	iks_tree_t *e1 = (*ast)->children->item;
	iks_ast_node_value_t *e1_n = e1->item;
	iks_tree_t *e2 = (*ast)->children->next->item;
	iks_ast_node_value_t *e2_n = e2->item;

	
	e1_n->temp.b.t = or_n->temp.b.t;
	e1_n->temp.b.f = label_generator();

	e2_n->temp.b.t = or_n->temp.b.t;
	e2_n->temp.b.f = or_n->temp.b.f;

	iks_list_concat(e1_n->code,label_code_generator(e1_n->temp.b.f));	
	iks_list_concat(e1_n->code,e2_n->code);	

	or_n->code = e1_n->code;
}

void code_if(iks_tree_t **ast){
	iks_ast_node_value_t *ctrl_flow_n = (*ast)->item;
	iks_ast_node_value_t *logic_expr_n = (*ast)->children->item;
	iks_ast_node_value_t *commands_n = (*ast)->children->next->item;

	commands_n->temp.next = ctrl_flow_n->temp.next;
	
	iks_list_concat(logic_expr_n->code,label_code_generator(logic_expr_n->temp.b.t));
	iks_list_concat(logic_expr_n->code,commands_n->code);
	ctrl_flow_n->code = logic_expr_n->code;	
}

void code_if_else(iks_tree_t **ast) {
	iks_ast_node_value_t *ctrl_flow_n = (*ast)->item;
	iks_ast_node_value_t *logic_expr_n = (*ast)->children->item;
	iks_ast_node_value_t *commands1_n = (*ast)->children->next->item;
	iks_ast_node_value_t *commands2_n = (*ast)->children->next->next->item;
	
	commands1_n->temp.next = ctrl_flow_n->temp.next;
	commands2_n->temp.next = ctrl_flow_n->temp.next;
	
	iks_list_concat(logic_expr_n->code,label_code_generator(logic_expr_n->temp.b.t));
	iks_list_concat(logic_expr_n->code,commands1_n->code);

	char *goto_code = strcat("goto ",logic_expr_n->temp.b.t);
	iks_list_concat(logic_expr_n->code,label_code_generator(goto_code));
	iks_list_concat(logic_expr_n->code,label_code_generator(logic_expr_n->temp.b.f));

	iks_list_concat(logic_expr_n->code,commands2_n->code);

	ctrl_flow_n->code = logic_expr_n->code;	
}

void code_generator(iks_tree_t **ast) {
	iks_list_t *code;
	code = new_iks_list();

	iks_ast_node_value_t *n;
	n = (*ast)->item;
	switch(n->type) {
		case IKS_AST_PROGRAMA:
		case IKS_AST_FUNCAO:
		case IKS_AST_IF:
			code_if(ast);
			break;
		case IKS_AST_IF_ELSE:
			code_if_else(ast);
			break;
//		case IKS_AST_DO_WHILE:
//			code_do_while(ast);
//			break;
//		case IKS_AST_WHILE_DO:
//			code_while_do(ast);
//			break;
		case IKS_AST_INPUT:
		case IKS_AST_OUTPUT:
		case IKS_AST_ATRIBUICAO:
		case IKS_AST_RETURN:
		case IKS_AST_BLOCO:
		case IKS_AST_IDENTIFICADOR:
		case IKS_AST_LITERAL:
		case IKS_AST_ARIM_SOMA:
		case IKS_AST_ARIM_SUBTRACAO:
		case IKS_AST_ARIM_MULTIPLICACAO:
		case IKS_AST_ARIM_DIVISAO:
		case IKS_AST_ARIM_INVERSAO:
		case IKS_AST_LOGICO_E:
		case IKS_AST_LOGICO_OU:
			code_or(ast);
			break;
		case IKS_AST_LOGICO_COMP_DIF:
		case IKS_AST_LOGICO_COMP_IGUAL:
		case IKS_AST_LOGICO_COMP_LE:
		case IKS_AST_LOGICO_COMP_GE:
		case IKS_AST_LOGICO_COMP_L:
		case IKS_AST_LOGICO_COMP_G:
		case IKS_AST_LOGICO_COMP_NEGACAO:
		case IKS_AST_VETOR_INDEXADO:
		case IKS_AST_CHAMADA_DE_FUNCAO:
		case IKS_AST_INDEFINIDO:
		default:
			break;
	}
}

char *label_generator(){
  char temp_label[LABEL_WIDTH];
  char *prefix, *new_label;
  
  prefix = malloc (sizeof (char) * LABEL_WIDTH);
  new_label = malloc (sizeof (char) * LABEL_WIDTH);

  strcpy (prefix, "l");
  sprintf(temp_label, "%d", label_ctrl); 
  label_ctrl++;
  
  strcpy (new_label, temp_label);

  new_label = strcat(prefix, new_label);

  return new_label;
}

char *register_generator(){
  char temp_register[REGISTER_WIDTH];
  char *prefix, *new_register;
  
  prefix = malloc (sizeof (char) * REGISTER_WIDTH);
  new_register = malloc (sizeof (char) * REGISTER_WIDTH);

  strcpy (prefix, "r");
  sprintf(temp_register, "%d", reg_ctrl); 
  reg_ctrl++;

  strcpy (new_register, temp_register);

  new_register = strcat(prefix, new_register);

  return new_register;
}