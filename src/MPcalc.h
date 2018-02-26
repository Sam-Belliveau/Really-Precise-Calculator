#include <mpfr.h>
#include <iostream>
#include <string>
#include <vector>

namespace calculator
{
    enum ops{openP, closeP, add, sub, mult, div, pow, sqrt, cbrt, root, abs, log10, log2, log, acos, asin, atan, cos, sin, tan, pi, e};

    namespace constants
    {
        const unsigned int opCount = 22;

        const unsigned int opL[opCount] = {1,1,1,1,1,1,1,4,4,4,3,6,6,3,4,4,4,3,3,3,2,1};
        const char* operators[opCount] = {"(", ")", "+", "-", "*", "/", "^", "sqrt", "cbrt", "root",
                                        "abs", "logTen", "logTwo", "log", "acos", "asin", "atan",
                                        "cos", "sin", "tan", "pi", "e"};

        const char* numberChars = "0123456789.";

        const unsigned int bitP = 1 << 16;
    }

    struct part
    {
        bool isNum; ops op;
        mpfr_t num;
    };
    static std::vector<part> input; unsigned int vLength = 0;

    namespace interperter
    {
        inline bool isDigit(const char a)
        {
            for(unsigned int i = 0; i < 11; i++){ if(a == constants::numberChars[i]){ return true; } }
            return false;
        }

        int buildInput(const char* in, const unsigned int length)
        {
            unsigned int vI = 0;

            for(unsigned int i = 0; i < length; i++)
            {
                // Character Is An Operator
                for(unsigned int j = 0; j < constants::opCount; j++)
                {
                    bool isOp = true;
                    const unsigned int opLength = constants::opL[j];
                    for(unsigned int offset = 0; offset < opLength; offset++)
                    {
                        if(in[i+offset] != constants::operators[j][offset])
                        {
                            isOp = false;
                            break;
                        }
                    }

                    if(isOp)
                    {
                        part blank;
                        input.push_back(blank);

                        input.back().isNum = false;

                        input[vI].op = static_cast<ops>(j);
                        i += opLength - 1;
                        vI++;
                    }
                }

                // Is a number/decimal
                if(isDigit(in[i]))
                {
                    part blank;
                    input.push_back(blank);

                    input.back().isNum = true;
                    mpfr_init2(input.back().num, constants::bitP);

                    // Create string of the number
                    std::string sNum = "";
                    while(isDigit(in[i]))
                    {
                        sNum += in[i++];

                        if(i == length)
                        {
                            break;
                        }
                    }

                    // Set Number
                    mpfr_set_str(input.back().num, sNum.c_str(), 10, MPFR_RNDN);
                    vI++; i--;
                }
            }
            return vI;
        }
    }

    namespace privateMath
    {
        void eraseIndex(const unsigned int i)
        {
            if(input[i].isNum)
            {
                mpfr_clear(input[i].num);
            }
            input.erase(input.begin()+i);
        }

        /** MATHMATICAL STUFF **/
        bool cVector(const unsigned int min, unsigned int max)
        {
            /** Parentheses **/
            bool done = false;
            while(!done)
            {
                done = true;

                unsigned int start, end, pCount = 0;

                for(unsigned int i = min; i < max; i++)
                {
                    if(!input[i].isNum)
                    {
                        if(input[i].op == ops::openP)
                        { if(pCount++ == 0) { start = i; } }

                        if(input[i].op == ops::closeP)
                        {
                            if(--pCount == 0)
                            {
                                done = false;

                                end = i;
                                eraseIndex(end--);
                                eraseIndex(start);
                                max -= 2; vLength -= 2;

                                unsigned int tempL = vLength;
                                if(cVector(start, end)){ return 1; }
                                max -= tempL - vLength; // Decreases Max

                                break;
                            }
                        }
                    }
                }
            }

            /** Constants **/
            for(unsigned int i = min; i < max; i++)
            {
                if(!input[i].isNum)
                {
                    if(input[i].op == ops::pi)
                    {
                        input[i].isNum = true;
                        mpfr_init2(input[i].num, constants::bitP);

                        mpfr_set_str(input[i].num, "3.14159265359", 10, MPFR_RNDN);
                    }
                    if(input[i].op == ops::e)
                    {
                        input[i].isNum = true;
                        mpfr_init2(input[i].num, constants::bitP);

                        mpfr_set_str(input[i].num, "2.71828182846", 10, MPFR_RNDN);
                    }
                }
            }

            /**NEGITIVES**/
            for(unsigned int i = min; i < max - 1; i++)
            {
                if(!input[i].isNum)
                {
                    // If the element before is not a number, make the element after negative
                    if(i == min && input[i + 1].isNum)
                    {
                        if(input[i].op == ops::sub)
                        {
                            mpfr_ui_sub(input[i+1].num, 0, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                        }
                    } else if(!input[i - 1].isNum && input[i + 1].isNum)
                    {
                        if(input[i].op == ops::sub)
                        {
                            mpfr_ui_sub(input[i+1].num, 0, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                        }
                    }
                }
            }

            /** One Num Op **/
            done = false;
            while(!done)
            {
                done = true;

                for(unsigned int i = min; i < max - 1; i++)
                {
                    if(!input[i].isNum)
                    {
                        if(input[i].op == ops::sqrt)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_sqrt(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::cbrt)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_cbrt(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::abs)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_abs(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::log10)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_log10(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::log2)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_log2(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::log)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_log(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::acos)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_acos(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::asin)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_asin(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::atan)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_atan(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::cos)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_cos(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::sin)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_sin(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                        if(input[i].op == ops::tan)
                        {
                            if(!input[i + 1].isNum){ return 1; }
                            done = false;

                            mpfr_tan(input[i+1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i);
                            max--; vLength--;
                            break;
                        }
                    }
                }
            }

            /** Pow **/
            done = false;
            while(!done)
            {
                done = true;

                for(unsigned int i = min + 1; i < max - 1; i++)
                {
                    if(!input[i].isNum)
                    {
                        if(input[i].op == ops::pow)
                        {
                            if(!input[i - 1].isNum || !input[i + 1].isNum) { return 1; }
                            done = false;

                            mpfr_pow(input[i-1].num, input[i-1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i); eraseIndex(i);
                            max -= 2; vLength -= 2;
                            break;
                        }
                        if(input[i].op == ops::root)
                        {
                            if(!input[i - 1].isNum || !input[i + 1].isNum) { return 1; }
                            done = false;

                            mpfr_rootn_ui(input[i-1].num, input[i+1].num, mpfr_get_si(input[i-1].num, MPFR_RNDN), MPFR_RNDN);

                            eraseIndex(i); eraseIndex(i);
                            max -= 2; vLength -= 2;
                            break;
                        }
                    }
                }
            }

            /** MULT (Including Undefined Op), DIV **/
            done = false;
            while(!done)
            {
                done = true;

                for(unsigned int i = min; i < max - 1; i++)
                {
                    if(!input[i].isNum && i != min)
                    {
                        if(input[i].op == ops::mult)
                        {
                            if(!input[i - 1].isNum || !input[i + 1].isNum) { return 1; }
                            done = false;

                            mpfr_mul(input[i-1].num, input[i-1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i); eraseIndex(i);
                            max -= 2; vLength -= 2;
                            break;
                        }

                        if(input[i].op == ops::div)
                        {
                            if(!input[i - 1].isNum || !input[i + 1].isNum) { return 1; }
                            done = false;

                            mpfr_div(input[i-1].num, input[i-1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i); eraseIndex(i);
                            max -= 2; vLength -= 2;
                            break;
                        }
                    } else {
                        if(input[i + 1].isNum)
                        {
                            done = false;

                            mpfr_mul(input[i].num, input[i].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i+1);
                            max--; vLength--;
                            break;
                        }
                    }
                }
            }

            /** ADD, SUBTRACT **/
            done = false;
            while(!done)
            {
                done = true;

                for(unsigned int i = min + 1; i < max - 1; i++)
                {
                    if(!input[i].isNum)
                    {
                        if(input[i].op == ops::add)
                        {
                            if(!input[i - 1].isNum || !input[i + 1].isNum) { return 1; }
                            done = false;

                            mpfr_add(input[i-1].num, input[i-1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i); eraseIndex(i);
                            max -= 2; vLength -= 2;
                            break;
                        }

                        if(input[i].op == ops::sub)
                        {
                            if(!input[i - 1].isNum || !input[i + 1].isNum) { return 1; }
                            done = false;

                            mpfr_sub(input[i-1].num, input[i-1].num, input[i+1].num, MPFR_RNDN);

                            eraseIndex(i); eraseIndex(i);
                            max -= 2; vLength -= 2;
                            break;
                        }
                    }
                }
            }
            return 0;
        }
    }

    /** What puts it all together **/
    char* calculate(const char* in, const unsigned int length)
    {
        vLength = interperter::buildInput(in, length);

        if(vLength == 0) { return "Error!"; }
        if(privateMath::cVector(0, vLength)){ return "Error!"; }
        if(!input[0].isNum){ return "Error!"; }

        char* raw;
        mpfr_exp_t e;
        raw = mpfr_get_str(NULL, &e, 10, 0, input[0].num, MPFR_RNDN);

        mpfr_sprintf(raw, "%.502RNf", input[0].num);

        unsigned int end = ((long) e) + 500;
        for(; raw[end] == '0'; --end)
        raw[end+1] = '\0';

        for(unsigned int i = 0; i < vLength; i++){ privateMath::eraseIndex(0); }
        input.clear();

        return raw;
    }
}
