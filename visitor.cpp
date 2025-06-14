#include <iostream>
#include "exp.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdentifierExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int StatementList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int Body::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    if(exp->value) cout << "true";
    else cout << "false";
    return 0;
}

int PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return 0;
}

void PrintVisitor::visit(AssignStatement* stm) {
    cout << stm->id << " = ";
    stm->rhs->accept(this);
    cout << ";";
}

void PrintVisitor::visit(PrintStatement* stm) {
    cout << "print(";
    stm->e->accept(this);
    cout << ");";
}

void PrintVisitor::visit(IfStatement* stm) {
    cout << "if ";
    stm->condition->accept(this);
    cout << " then" << endl;
    stm->then->accept(this);
    if(stm->els){
        cout << "else" << endl;
        stm->els->accept(this);
    }
    cout << "endif";
}

void PrintVisitor::imprimir(Program* program){
    program->body->accept(this);
};




void PrintVisitor::visit(VarDec* stm){
    cout << "var ";
    cout << stm->type;
    cout << " ";
    for(auto i: stm->vars){
        cout << i;
        if(i != stm->vars.back()) cout << ", ";
    }
    cout << ";";
}

void PrintVisitor::visit(VarDecList* stm){
    for(auto i: stm->vardecs){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(StatementList* stm){
    for(auto i: stm->stms){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Body* stm){
    stm->vardecs->accept(this);
    cout << endl;
    stm->slist->accept(this);
}


void GenCodeVisitor::gencode(Program* program) {
    cout << ".data" << endl;
    cout << "print_fmt: .string \"%ld \\n\" " << endl;
    cout << ".text " << endl;
    cout << ".globl main " << endl;
    cout << "main: " << endl;
    cout <<" pushq %rbp" << endl;
    cout << " movq %rsp, %rbp" << endl;
    cout << " subq $" << program->contar()*8 << ", %rsp" << endl;
    program->body->accept(this);
    cout << " movl $0, %eax " << endl;
    cout << " leave" << endl;
    cout << " ret" << endl;
    cout << ".section .note.GNU-stack,\"\",@progbits" << endl;
}



int GenCodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << " pushq %rax"<<endl;
    switch(Exp::binopToChar(exp->op)) {
        case '+': {
            exp->right->accept(this);
            cout << " movq %rax, %rcx"<<endl;
            cout << " popq %rax"<<endl;
            cout << " addq %rcx, %rax"<<endl;
            break;
        }
        case '-': {
            exp->right->accept(this);
            cout << " movq %rax, %rcx"<<endl;
            cout << " popq %rax"<<endl;
            cout << " subq %rcx, %rax"<<endl;
            break;
        }
        case '*': {
            exp->right->accept(this);
            cout << " movq %rax, %rcx"<<endl;
            cout << " popq %rax"<<endl;
            cout << " mulq %rcx, %rax"<<endl;
            break;
        }
        case '<': {
            exp->right->accept(this);
            cout << " movq %rax, %rcx"<<endl;
            cout << " popq %rax"<<endl;
            cout << " cmpq %rcx, %rax"<<endl;
            cout << " movl $0, %eax"<<endl;
            cout << " setl %al" << endl;
            cout << " movzbq %al, %rax" << endl;
            break;
        }
    }
    return 0;
}
int GenCodeVisitor::visit(NumberExp* exp) {
    cout << " movq $"<< exp->value<< ", %rax"<<endl;
    return 0;
}
int GenCodeVisitor::visit(BoolExp* exp){   return 0;}
int GenCodeVisitor::visit(IdentifierExp* exp) {
    cout << " movq "<< memoria[exp->name] <<"(%rbp), %rax" <<endl;
    return 0;
}
void GenCodeVisitor::visit(AssignStatement* stm) {
    stm->rhs->accept(this);
    cout << " movq %rax,"<< memoria[stm->id] <<" (%rbp)"<<endl;
}
void GenCodeVisitor::visit(PrintStatement* stm) {
    stm->e->accept(this);
    cout << " movq %rax, %rsi" << endl;
    cout << " leaq print_fmt(%rip), %rdi" << endl;
    cout << " movl $0, %eax" << endl;
    cout << " call printf@PLT" << endl;
}
void GenCodeVisitor::visit(IfStatement* stm) {
    stm->condition->accept(this);
    cout << " cmpq $0, %rax" << endl;
    cout << " je else_1"<< endl;
    stm->then->accept(this);
    cout << " jmp endif_1" << endl;
    cout << " else_1:" << endl;
    stm->els->accept(this);
    cout << " endif_1:" << endl;
}

void GenCodeVisitor::visit(VarDec* stm) {
    for(auto i:stm->vars) {
        memoria[i] = -8*cantidad;
        cantidad++;
    }
}
void GenCodeVisitor::visit(VarDecList* stm) {
    for(auto i: stm->vardecs) {
        i->accept(this);
    }
}
void GenCodeVisitor::visit(StatementList* stm) {
    for(auto i:stm->stms) {
        i->accept(this);
    }
}
void GenCodeVisitor::visit(Body* b) {
    b->vardecs->accept(this);
    b->slist->accept(this);
}