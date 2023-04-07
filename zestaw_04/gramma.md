<request-message> :== <request-list> <endl>

<response-message> :== <response-list> <endl>

<request-list> ::=
    | <expression> <endl>
    | <expression> <endl> <request-list>

<response-list> ::=
    | <result> <endl>
    | <result> <endl> <response-list>

<expression> ::=
    | <number>
    | <expression> <operator> <expression>

<result> ::=
    | <number>
    | "ERROR"

<number> ::=
    | <digit>
    | <digit> <number>

<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

<operator> ::= "+" | "-"

<endl> ::= "\r\n"
