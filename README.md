# Compiler construction

* Câu hỏi cho parser
    - Function tokenToString() dựa vào ```tokenType``` để parse 1 token ra string để làm gì?

        ``` c
          switch (tokenType) {
            case TK_NONE: return "None";
            ...
          }
        ```
    -