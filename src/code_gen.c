#include <stdint.h>
#include <byteswap.h>
#include "error.h"
#include "code_gen.h"

#define GEN_RIGHT(imm) gen_instr(INST_RIGHT, imm)  // Move data ptr right
#define GEN_LEFT(imm)  gen_instr(INST_RIGHT, -imm) // Move data ptr left
#define GEN_ADD(imm)   gen_instr(INST_ADD, imm)    // Add to value at data ptr
#define GEN_SUB(imm)   gen_instr(INST_ADD, -imm)   // Sub from value at data ptr
#define GEN_BNZ(imm)   gen_instr(INST_BNZ, imm)    // Branch if not zero

#define GEN_BNZ_LOOP(instrs) {            \
    Word tmp_instr_cnt = instr_cnt;       \
    instrs                                \
    GEN_BNZ(instr_cnt - tmp_instr_cnt); } \


// Word length
typedef uint32_t Word;

// Instruction op-codes
enum {
    INST_RIGHT = 0x0,
    INST_ADD   = 0x1,
    INST_BNZ   = 0x2
};

static void gen_ast(AstNode *ast);

static FILE *outfile;
static Word instr_cnt = -1;

static void gen_instr(char opcode, Word imm)
{
    Word instr = (opcode << 30) | (imm & 0x3FFFFFFF);

    instr = __bswap_constant_32(instr); // Little-endian to big-endian

    fwrite(&instr, 4, 1, outfile);
    instr_cnt++;
}

// Set the value at the data pointer to zero
// Note: If the value at the data pointer is allready zero this causes and
//       integer overflow and can be very inefficient. I need to figure out a
//       way to wix this.
static void zero_at_dp()
{
    //GEN_BNZ(8);
    // TODO: unconditional jump 12
    GEN_SUB(1);
    GEN_BNZ(-4);
}

/* Stack */

// TODO: currently currently the stack only works with word sized values
static void stack_push(Word value)
{
    // Set the value
    zero_at_dp();
    GEN_ADD(value);
    // Increment the data pointer
    GEN_RIGHT(sizeof(Word));
}

static void stack_pop()
{
    GEN_LEFT((unsigned)sizeof(Word));
}

// Move the data pointer right by one stack object.
// Note: Currently the size of a stack object is assumed to be 32 bits but this
//       will change when I get round to implementing things like structures.
static void stack_right(int amm)
{
    GEN_RIGHT(sizeof(Word) * amm);
}

// Move the data pointer left by one stack object.
static void stack_left(int amm)
{
    GEN_LEFT(sizeof(Word) * amm);
}

static void gen_binary_op(AstNode *binary_op)
{
    gen_ast(binary_op->binary_left);
    gen_ast(binary_op->binary_right);

    switch (binary_op->binary_op) {
    case OP_PLUS:
        // Set the data pointer to the last value on the stack.
        // This should be result of the RHS expression.
        stack_left(1);

        GEN_BNZ_LOOP (
            GEN_SUB(1);    // Subtract 1 from the RHS
            stack_left(1);
            GEN_ADD(1);    // Add one to the LHS
            stack_right(1);
        )

        stack_pop();
        break;
    case OP_MINUS:
        break;
    case OP_MULT:
        break;
    case OP_DIV:
        break;
    case OP_MODULO:
        break;
    case OP_EQUALITY:
        break;
    case OP_NOT_EQUAL:
        break;
    case OP_GREATER_THAN:
        break;
    case OP_LESS_THAN:
        break;
    case OP_LESS_THAN_EQUAL:
        break;
    case OP_GREATER_THAN_EQUAL:
        break;
    case OP_LOGICAL_AND:
        break;
    case OP_LOGICAL_OR:
        break;
    case OP_BITWISE_NOT:
        break;
    case OP_BITWISE_OR:
        break;
    case OP_BITWISE_XOR:
        break;
    case OP_BITWISE_AND:
        break;
    case OP_SHIFT_LEFT:
        break;
    case OP_SHIFT_RIGHT:
        break;
    case OP_ASSIGN_EQUAL:
        break;
    case OP_ASSIGN_MINUS:
        break;
    case OP_ASSIGN_PLUS:
        break;
    case OP_ASSIGN_MULT:
        break;
    case OP_ASSIGN_DIV:
        break;
    case OP_ASSIGN_MODULO:
        break;
    case OP_ASSIGN_AND:
        break;
    case OP_ASSIGN_OR:
        break;
    case OP_ASSIGN_XOR:
        break;
    case OP_ASSIGN_SHIFT_LEFT:
        break;
    case OP_ASSIGN_SHIFT_RIGHT:
        break;
    case OP_COMMA:
        break;
    }
}

// Recursive function for generating code from the ast
static void gen_ast(AstNode *ast)
{
    switch (ast->node_type) {
    case AST_FUNCTION_DEF:
        // TODO
        gen_ast(ast->func_body);
        break;
    case AST_FUNC_DECLARATION:
        break;
    case AST_FUNC_CALL:
        break;
    case AST_RETURN_STMT:
        break;
    case AST_EXPR_STMT:
        gen_ast(ast->expression);
        break;
    case AST_COMPOUND_STMT:
        for (int i = 0; i < ast->statements->length; i++)
            gen_ast(ast->statements->items[i]);
        break;
    case AST_WHILE_STMT:
        break;
    case AST_DO_WHILE_STMT:
        break;
    case AST_IF_STMT:
        break;
    case AST_GOTO_STMT:
        break;
    case AST_BREAK_STMT:
        break;
    case AST_CONTINUE_STMT:
        break;
    case AST_LABEL_STMT:
        break;
    case AST_SWITCH_STMT:
        break;
    case AST_CASE_STMT:
        break;
    case AST_DEFAULT_STMT:
        break;
    case AST_DECLARATION:
        break;
    case AST_DECLARATOR_HEAD:
        break;
    case AST_TYPEDEF:
        break;
    case AST_DECL_LIST:
        break;
    case AST_CONDITIONAL_EXPR:
        break;
    case AST_STRUCT_MEMBER_ACCESS:
        break;
    case AST_INTEGER_CONST:
        stack_push(ast->integer_const);
        break;
    case AST_FLOAT_CONST:
        break;
    case AST_STR_LIT:
        break;
    case AST_IDENTIFIER:
        break;
    case AST_CAST_EXPR:
        break;
    case AST_BINARY_OP:
        gen_binary_op(ast);
        break;
    case AST_UNARY_OP:
        break;
    case AST_DATA_TYPE:
        break;
    }
}

void code_gen(AstNode *ast, const char *out_filename)
{
    outfile = fopen(out_filename, "wb");
    if (outfile == NULL)
        error(out_filename, -1, "failed to open \"%s\".", out_filename);

    gen_ast(ast);

    fclose(outfile);
}
