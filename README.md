# ManCityLang
ManCityLang - C-подобный язык программирования, посвящённый треблу Манчестер Сити в сезоне 2022-2023.

## Запуск компилятора

Для сборки и запуска компилятора выполните следующие команды:
```
git clone --recursive https://github.com/GadeevDmitry/my_x86-64_compiler.git
cd my_x86-64_compiler
make jit
./jit filename
```
## Необходимая информация для пользователей
1. Все переменные и числа имеют 64-битный знаковый целочисленный тип.
2. Все функции должны иметь возвращаемое значение перед закрывающей фигурной скобкой.
3. Доступны арифметические операции `+, -, *, /`.
4. Доступны логические операции `>, <, >=, <=, &&, ||, !`.
5. Доступна унарная операция `^`, которая извлекает квадратный корень из операнда.
6. Все переменные и константы, содержащиеся в программе, умножены на `SCALE = 100`. Это сделано для того чтобы вести приближенную арифметику с плавающей точкой, сохраняя 2 дробных десятичных разряда.
7. Результаты логических операций не нормируются относительно `SCALE`, поэтому значение `true` эквивалентно `1/SCALE = 0.01` (`false` эквивалентно `0`).
8. Имя главной функции - `MAN_CITY`.
9. Грамматика ManCityLang (примеры в папке examples):

   ```
   general      = { var_decl_op | func_decl }+

   var_decl_op  = var_decl ';'
   var_decl     = long_token VAR_NAME

   func_decl    = long_token FUNC_NAME '(' func_args? ')' '{' operators '}'
   func_args    = var_decl {',' var_decl }*

   operators    = { var_decl_op | assign_op | input | output | if | while | func_call_op | return }*

   assign_op    = assign ';'
   assign       = lvalue '=' rvalue

   func_call_op = func_call ';'
   func_call    = FUNC_NAME '(' func_params? ')'
   func_params  = rvalue {',' rvalue }*

   input        = input_token  lvalue ';'
   output       = output_token rvalue ';'

   if           = if_token '(' rvalue ')' '{' operators '}' else?
   else         = else_token '{' operators '}'

   while        = while_token '(' rvalue ')' '{' operators '}'
   return       = return_token rvalue ';'

   rvalue       = assign_part? op_or
   assign_part  = (lvalue '=')+

   op_or        = op_and    {'||'                op_and    }*
   op_and       = op_equal  {'&&'                op_equal  }*
   op_equal     = op_cmp    {['==' '!=']         op_cmp    }*
   op_cmp       = op_add_sub{['>=' '<=' '>' '<'] op_add_sub}*
   op_add_sub   = op_mul_div{['+' '-']           op_mul_div}*
   op_mul_div   = op_not    {['*' '/']           op_not    }*
   op_not       = {'!'}? op_sqrt
   op_sqrt      = {'^'}? operand
   operand      = '(' rvalue ')' | func_call | rvalue_token

   rvalue_token = number | lvalue
   number       = {'-'}?['0'-'9']+

   lvalue       = VAR_NAME
   ```

   В таблице приведены значения ключевых слов из грамматики:

   |long_token|    if_token    |  else_token  |while_token|   return_token   |input_token|output_token|
   |:--------:|:--------------:|:------------:|:---------:|:----------------:|:---------:|:----------:|
   |  TRABLE  |CHAMPIONS_LEAGUE|PREMIER_LEAGUE| GUARDIOLA |MANCHESTER_IS_BLUE| DE_BRUYNE |   RODRI    |

   Фанаты Юнайтед могут поменять ключевые слова. Для этого вам нужно изменить названия в 1-ом столбце массива, изображенного ниже, который находится [здесь](frontend/tokenizer/tokenizer_static.h).

   ```C++
   static token_name KEY_NAMES[] =
   {
        {"TRABLE"            , "long"    , 0},

        {"CHAMPIONS_LEAGUE"  , "if"      , 0},
        {"PREMIER_LEAGUE"    , "else"    , 0},
        {"GUARDIOLA"         , "while"   , 0},

        {"MANCHESTER_IS_BLUE", "return"  , 0},

        {"DE_BRUYNE"         , "input"   , 0},
        {"RODRI"             , "output"  , 0},
   };
   ```
   Но будьте внимательны! После изменения токенов старые исходные коды перестанут компилироваться.

## Устройство компилятора

Работу компилятора можно разбить на 2 основные части: **frontend** и **backend**. Задача `frontend` - преобразование исходного кода программы в абстрактное синтаксическое дерево (abstract syntax tree, AST). Листья AST сопоставлены с операндами, а остальные вершины - с операторами. Задача `backend` - генерация машинного кода из AST.

### frontend

`frontend` в свою очередь делится еще на 2 этапа:

* [Лексический анализ](frontend/tokenizer/) - разбиение исходного кода на последовательность токенов - ключевых слов, имен переменных и функций, символов-разделителей и т.д.
* [Синтаксический анализ](frontend/parser/) - проверка кода на соответствие грамматическим правилам, построение AST с помощью рекурсивного спуска.

### backend
Трансляция в машинный код осуществляется в 3 этапа:
1. [**AST -> IR**](backend/IR_translator/IR_translator.cpp)

  На данном этапе синтаксическое дерево обходится в порядке *postorder*, генерируя IR - массив структур [IR_node](backend/IR/IR.h), в котором каждая вершина является командой абстрактного процессора. Преимущество IR заключается в том, что он не привязан к конкретной архитектуре.

2. [**IR -> нативный код**](backend/x64_translator/x64_translator.cpp)

  Далее IR переводится в последовательность инструкций реальной архитектуры, в нашем случае - x64. Для представления инструкции используется структура [x64_node](backend/x64/x64.h).

3. [**нативный код -> бинарный код**](backend/binary_translator/bin_translator.cpp)

  Последний этап - генерация бинарного кода для дальнейшей конвертации в файл или исполнения.

#### IR

В данной реализации IR рассчитан на абстрактный стековый процессор и представляет собой массив структур [IR_node](backend/IR/IR.h), в котором каждый элемент соответствует отдельной команде IR:

```C++
struct IR_node
{
    IR_CMD type;                // тип команды

    struct
    {
        bool is_reg_arg;        // true, если операнд содержит регистр
        bool is_mem_arg;        // true, если операнд ссылается на память
        bool is_imm_arg;        // true, если операнд содержит непосредственное значение
    }
    is_arg;

    unsigned char reg_num;      // номер регистра
    int           imm_val;      // непосредственное значение
};
```

| IR_CMD                                                        | Описание                                                                                   |
|---------------------------------------------------------------|--------------------------------------------------------------------------------------------|
| ADD, SUB, MUL, DIV, SQRT, OR, AND, ARE_EQ, NOT_EQ, LESS, MORE, LESS_EQ, MORE_EQ, NOT | Извлечение операндов из стека -> операция -> добавление результата в стек |
| Jcc  `imm`                             | Извлечение 2-х операндов из стека -> операция `cc` -> jump, если результат равен true             |
| JMP  `imm`                             | Безусловный переход. Поле `imm` содержит номер IR_node в массиве для перехода                     |
| CALL `imm`                             | Переход с сохранением адреса возврата. Поле `imm` содержит номер IR_node в массиве для перехода   |
| RET                                    | Возврат к адресу, сохраненному последней функцией CALL                                            |
| PUSH `reg/[reg]/imm/[imm]/[reg+imm]`   | Добавление операнда в стек                                                                        |
| POP  `reg/[reg]/[imm]/[reg+imm]`       | Извлечение значения из стека с сохранением в операнд                                              |
| IN                                     | Ввод  + добавление в стек числа                                                                   |
| OUT                                    | Вывод + извлечение числа из стека                                                                 |

Замечания по таблице:
*  В бинарных операторах на вершине стека находится правый (второй) операнд.
*  В описании под словом стек подразумевается абстрактный стек - на его реализацию ограничений не накладывается.

## LLVM-frontend

В дополнение к компилятору был написан [llvm](https://github.com/llvm/llvm-project)-frontend, который преобразует исходный код в [llvm IR](https://llvm.org/docs/LangRef.html) - стандартизированное промежуточное представление.

### Запуск
Для сборки и запуска llvm-frontend выполните следующие команды:
```
make LlvmJit
./LlvmJit [source filename] 2> [output filename]
```
В выходном файле появится сгенерированный llvm-ir в текстовом представлении.

### Устройство llvm-frontend
llvm-frontend состоит из двух частей: [parser](frontend/llvm/parser/) и [IR-translator](frontend/llvm/IR/).

#### Parser
Для реализации llvm-frontend было принято решение написать новый парсер, который лучше предыдущего по следующим причинам:
  * [Архитектура AST](ast/llvm/) реализована с помощью техники ООП - это позволяет использовать разные производные классы AST-вершин для разных синтаксических конструкций, вместо одной структуры с union-ом внутри.

  * Для парсинга бинарных выражений была использована техника ["Operator-precedence parser"](https://en.wikipedia.org/wiki/Operator-precedence_parser). Она хорошо ложиться на синтаксис бинарных выражений данного языка и проще в реализации, чем рекурсивный спуск. Пример использования данной техники с пояснениями также можно найти в [llvm-Kaleidoscope](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl02.html).

  * Также, AST-вершины хранят имена объектов - это облегчает взаимодействие с llvm-таблицами символов, и текстовое представление IR cодержит те же имена, что и в исходном коде.

#### IR
Генерация IR осуществляется с помощью (библиотеки llvm)[https://llvm.org/doxygen/]. Вот как может выглядеть IR для следующего исходного кода:
```
TREBLE factorial ( TREBLE n )
{
    CHAMPIONS_LEAGUE ( n <  0 )           { MANCHESTER_IS_BLUE 0 ; }
    CHAMPIONS_LEAGUE ( n == 0 || n == 1 ) { MANCHESTER_IS_BLUE 1 ; }

    MANCHESTER_IS_BLUE n * factorial ( n - 1 ) ;
}

TREBLE N ;
TREBLE MAN_CITY ( )
{
#   DE_BRUYNE  N ;
#   RODRI
    factorial ( N ) ;

    MANCHESTER_IS_BLUE 0 ;
}

```
```
define internal double @factorial(double %n) {
entry:
  %n1 = alloca double, align 8
  %n2 = load double, double* %n1, align 8
  %"<" = fcmp ult double %n2, 0.000000e+00
  %0 = uitofp i1 %"<" to double
  %"if cond" = fcmp one double %0, 0.000000e+00
  br i1 %"if cond", label %then, label %"if cont"

then:                                             ; preds = %entry
  ret double 0.000000e+00

1:                                                ; No predecessors!
  br label %"if cont"

"if cont":                                        ; preds = %1, %entry
  %n3 = load double, double* %n1, align 8
  %"==" = fcmp ueq double %n3, 0.000000e+00
  %2 = uitofp i1 %"==" to double
  %n4 = load double, double* %n1, align 8
  %"==5" = fcmp ueq double %n4, 1.000000e+00
  %3 = uitofp i1 %"==5" to double
  %"lhs: double to int" = fptoui double %2 to i1
  %"rhs: double to int" = fptoui double %2 to i1
  %"||" = select i1 %"lhs: double to int", i1 true, i1 %"rhs: double to int"
  %4 = uitofp i1 %"||" to double
  %"if cond6" = fcmp one double %4, 0.000000e+00
  br i1 %"if cond6", label %then7, label %"if cont8"

then7:                                            ; preds = %"if cont"
  ret double 1.000000e+00

5:                                                ; No predecessors!
  br label %"if cont8"

"if cont8":                                       ; preds = %5, %"if cont"
  %n9 = load double, double* %n1, align 8
  %n10 = load double, double* %n1, align 8
  %- = sub double %n10, 1.000000e+00
  %factorial = call double @factorial(double %-)
  %"*" = mul double %n9, %factorial
  ret double %"*"

6:                                                ; No predecessors!
}

@N = external global double
define internal double @MAN_CITY() {
entry:
  %N = load double, double* @N, align 8
  %factorial = call double @factorial(double %N)
  ret double 0.000000e+00

0:                                                ; No predecessors!
}
```
