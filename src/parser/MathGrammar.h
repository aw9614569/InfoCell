#pragma once

#include <parser/Grammar.h>
#include <parser/Value.h>

class CMathGrammar : public Grammar
{
    public:
        CMathGrammar() : Grammar("Math")
        {
            Digit | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';

            DecimalLiteral
                & Digit
                & Digit % Any
                ;

            Prec1 | Mul | Div | DecimalLiteral;

            Mul & DecimalLiteral
                & '*'
                & Prec1
                ;

            Div & DecimalLiteral
                & '/'
                & Prec1
                ;

            Prec2 | Add | Sub | Prec1;

            Add & Prec1
                & '+'
                & Prec2
                ;

            Sub & Prec1
                & '-'
                & Prec2
                ;

            Expr | Equal | NotEqual | Prec2;

            Equal
                & Prec2
                & "=="
                & Expr
                ;

            NotEqual
                & Prec2
                & "!="
                & Expr
                ;
        }
        TRule Digit = Or("Digit");
        TRule DecimalLiteral = Obj<DecimalLiteralValue>("DecimalLiteral");
        TRule Expr = Or("Expr");
        TRule Equal = Obj<BinaryOpValue>("Equal");
        TRule NotEqual = Obj<BinaryOpValue>("NotEqual");
        TRule Prec1 = Or("Prec1");
        TRule Mul = Obj<BinaryOpValue>("Mul");
        TRule Div = Obj<BinaryOpValue>("Div");
        TRule Prec2 = Or("Prec2");
        TRule Add = Obj<BinaryOpValue>("Add");
        TRule Sub = Obj<BinaryOpValue>("Sub");
};