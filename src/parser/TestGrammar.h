#pragma once
#include <parser/Grammar.h>

class TestGrammar : public Grammar
{
    public:
        TestGrammar() : Grammar("Test")
        {
            testOr | strABC | strDEF;
            strABC << "ABC";
            strDEF << "DEF";

            testOptionalBasic1
                & Char('A') % Optional
                & 'B'
                & 'C'
                ;

            testOptionalBasic2
                & Char('A') % Optional
                & Char('B') % Optional
                & 'C'
                ;

            testOptionalBasic3
                & Str("AA") % Optional
                & Str("BB") % Optional
                & 'C'
                ;

            testOptionalBasic4
                & strABC % Any
                ;

            testAny1
                | DecimalLiteral
                | testAny1Tail
                ;

            testAny1Tail
                & '1'
                & DecimalLiteral
                ;

            Digit | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';

            DecimalLiteral
                & Digit
                & Digit % Any;
                ;
        }

        TRule testOr = Or("testOr");
        TRule strABC = And("ABC");
        TRule strDEF = And("DEF");
        TRule testOptionalBasic1 = And("testOptionalBasic1");
        TRule testOptionalBasic2 = And("testOptionalBasic2");
        TRule testOptionalBasic3 = And("testOptionalBasic3");
        TRule testOptionalBasic4 = And("testOptionalBasic4");
        TRule testAny1 = Or("testAny1");
        TRule testAny1Tail = And("testAny1Tail");
        TRule Digit = Or("Digit");
        TRule DecimalLiteral = And("DecimalLiteral");
};