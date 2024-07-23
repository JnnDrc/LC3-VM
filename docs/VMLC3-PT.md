
# Maquina Virtual LC-3

Esse programa é uma maquina virtual
para a arquitetura LC-3.
esse projeto é baseado nesses artigos: [um](https://www.andreinc.net/2021/12/01/writing-a-simple-vm-in-less-than-125-lines-of-c#virtual-machines), [dois](https://www.jmeiners.com/lc3-vm/)

## O LC-3 e a arquitetura

O Little Computer 3 ou LC-3 é uma
simples arquitetura de 16 bits, usada principalmente
para ensinar ciência da computação e
programação em baixo nível.

O LC-3 tem 10 registradores de 16-bit,
8 para proposito geral (R0 ... R7),
o Registrador de Contador de Programa(RPC),
e o Registrador de Bandeira da CPU (RCND).  

Ela suporta as operações de SOMA(ADD),
E(AND) e NÃO(NOT) bit-a-bit com inteiros de 16 bits,
possui suporte para complemento de dois.

Sua ALU tem as três bandeiras Positivo(P),Negativo(N) e Zero(Z).

Também conta com 128KB de memória, separados em 64K words, o
endereço inicial para escrever programa na VM,por padrão, é o 0x3000.

## Instruction set

A sintaxe geral das instruções é:

![instrução geral](instr.drawio.png)

<table>
    <tr>
        <th>Instrução</th>
        <th>Código OP(hex)</th>
        <th>Código OP(bin)</th>
        <th>Sintaxe</th>
        <th>Função</th>
    </tr>
    <tr>
        <td>BR</td>
        <td>0x0</td>
        <td>0b0000</td>
        <td>0000|NZP3|OFF9</td>
        <td>Ramificação condicional</td>
    </tr>
    <tr>
        <td>ADD<sub>1</sub></td>
        <td>0x1</td>
        <td>0b0001</td>
        <td>0001|DR1(3)|SR1(3)|000|SR2(3)</td>
        <td>Operação de adição com SR1 e SR2</td>
    </tr>
    <tr>
        <td>ADD<sub>2</sub></td>
        <td>0x1</td>
        <td>0b0001</td>
        <td>0001|DR1(3)|SR1(3)|1|IMM5</td>
        <td>Operação de adição com SR1 e IMM</td>
    </tr>
    <tr>
        <td>LD</td>
        <td>0x2</td>
        <td>0b0010</td>
        <td>0010|DR1(3)|OFF9</td>
        <td>Carrega em DR1 RPC + OFFSET</td>
    </tr>
    <tr>
        <td>ST</td>
        <td>0x3</td>
        <td>0b0011</td>
        <td>0011|SR(3)|OFF9</td>
        <td>Armazena RPC + OFFSET</td>
    </tr>
    <tr>
        <td>JSR</td>
        <td>0x4</td>
        <td>0b0100</td>
        <td>0100|1|OFF11</td>
        <td>Salta para sub-rotina (RPC + OFFSET)</td>
    </tr>
    <tr>
        <td>JSRR</td>
        <td>0x4</td>
        <td>0b0100</td>
        <td>0100|000|BASER3|000000</td>
        <td>Salta para sub-rotina (endereço em BASER, Registrador Base)</td>
    </tr>
    <tr>
        <td>AND<sub>1</sub></td>
        <td>0x5</td>
        <td>0b0101</td>
        <td>0101|DR1(3)|SR1(3)|000|SR2(3)</td>
        <td>Operação de E lógico</td>
    </tr>
    <tr>
        <td>AND<sub>2</sub></td>
        <td>0x5</td>
        <td>0b0101</td>
        <td>0101|DR1(3)|SR1(3)|1|IMM5</td>
        <td>Operação de E lógico</td>
    </tr>
    <tr>
        <td>LDR</td>
        <td>0x6</td>
        <td>0b0110</td>
        <td>0110|DR1(3)|BASER3|OFF6</td>
        <td>Carrega em DR1 BASER + OFFSET</td>
    </tr>
    <tr>
        <td>STR</td>
        <td>0x7</td>
        <td>0b0111</td>
        <td>0111|DR1(3)|BASER3|OFF6</td>
        <td>Armazena DR1 no endereço BASER + OFFSET</td>
    </tr>
    <tr>
        <td>RTI</td>
        <td>0x8</td>
        <td>0b1000</td>
        <td>Não Implementado</td>
        <td>Retorna da interrupção</td>
    </tr>
    <tr>
        <td>NOT</td>
        <td>0x9</td>
        <td>0b1001</td>
        <td>1001|DR1(3)|SR1(3)|111111</td>
        <td>Negação bit a bit</td>
    </tr>
    <tr>
        <td>LDI</td>
        <td>0xA</td>
        <td>0b1010</td>
        <td>1010|DR1(3)|OFF9</td>
        <td>Carrega em DR1 dado no endereço armazenado em RPC + OFFSET</td>
    </tr>
    <tr>
        <td>STI</td>
        <td>0xB</td>
        <td>0b1011</td>
        <td>1011|SR(3)|OFF9</td>
        <td>Armazena SR no endereço armazenado em RPC + OFFSET</td>
    </tr>
    <tr>
        <td>JMP</td>
        <td>0xC</td>
        <td>0b1100</td>
        <td>1100|000|BASER3|000000</td>
        <td>Salta para o endereço em BASER (Registrador Base)</td>
    </tr>
    <tr>
        <td>RES</td>
        <td>0xD</td>
        <td>0b1101</td>
        <td>1101|000000000000</td>
        <td>Não existe no LC-3 oficial, Reinicia a CPU</td>
    </tr>
    <tr>
        <td>LEA</td>
        <td>0xE</td>
        <td>0b1110</td>
        <td>1110|DR1(3)|OFF9</td>
        <td>Carrega o Endereço Eficaz (Carrega o endereço de RPC + OFFSET para DR1)</td>
    </tr>
    <tr>
        <td>TRP</td>
        <td>0xF</td>
        <td>0b1111</td>
        <td>1111|0000||TRPVECT8</td>
        <td>Armadilha, executa função integrada baseada em TRPVECT</td>
    </tr>
</table>

### Vetor Trap

A operação Trap(TRP) é usada para I/O e terminar o programa,
o que ela faz é baseada no seu único argumento de 8bit, TRPVECT, nesta VM,
há 10 sub-rotinas para TRPVECT:

<table>
    <tr>
        <th>Sub-rotina Trap</th>
        <th>TRPVECT</th>
        <th>Função</th>
    </tr>
        <td>trpgetc</td>
        <td>0x20</td>
        <td>Lê um char do teclado e armazena ela no R0</td>
    </tr>
    <tr>
        <td>trpout</td>
        <td>0x21</td>
        <td>Escreve o char armazenado no R0 no console</td>
    </tr>
    <tr>
        <td>trpputs</td>
        <td>0x22</td>
        <td>Escreve uma string no console, strings são caracteres
         em posições continuas de memória,que inicia no endereço armazenado start em R0, e acaba quando
         encontra um char '\0'</td>
    </tr>
    <tr>
        <td>trpin</td>
        <td>0x23</td>
        <td>Lê um char do teclado, armazena ela no R0 e escreve ele
        no console</td>
    </tr>
    <tr>
        <td>trpputsp</td>
        <td>0x24</td>
        <td>Escreve uma string de chars empacotados no console, strings são caracteres
         em posições continuas de memória,que inicia no endereço armazenado start em R0, e acaba quando
         encontra um char '\0', chars empacotados são dois chars em uma word(2 bytes)
         </td>
    </tr>
    <tr>
        <td>trphlt</td>
        <td>0x25</td>
        <td>Termina o programa, fecha a VM</td>
    </tr>
    <tr>
        <td>trpin16</td>
        <td>0x26</td>
        <td>Lê uma word do teclado e armazena ela no R0
        </td>
    </tr>
    <tr>
        <td>trpoutu16</td>
        <td>0x27</td>
        <td>Escreve a word sem sinal armazenada em R0 no console</td>
    </tr>
    <tr>
        <td>trpouti16</td>
        <td>0x28</td>
        <td>Escreve a word com sinal armazenada em R0 no console</td>
    </tr>
    <tr>
        <td>trpputsc</td>
        <td>0x29</td>
        <td>Escreve uma string colorida no console, strings são caracteres
         em posições continuas de memória,que inicia no endereço armazenado start em R0, e acaba quando
         encontra um char '\0', As cores de texto e de fundo, são armazenadas no byte superior,
         cada uma ocupando um nibble(4 bits),o primeiro nibble sendo a cor de texto, e o segundo a
         cor de fundo, as cores suportadas são as 8 cores básicas do padrão ASCII.
         (obs: o primeiro nibble é o com os menores valores[8-11],e o segundo é o com os maiores
         valores[12-15])
         </td>
    </tr>
</table>

### Cores trpputsc

<table>
    <tr>
        <th>Cor</th>
        <th>Código ASCII(texto/fundo)(dec)</th>
        <th>Código de maquina da VM(hex)</th>
    </tr>
    <tr>
        <td>Preto</td>
        <td>30/40</td>
        <td>0x0</td>
    </tr>
    <tr>
        <td>Vermelho</td>
        <td>31/41</td>
        <td>0x1</td>
    </tr>
    <tr>
        <td>Verde</td>
        <td>32/42</td>
        <td>0x2</td>
    </tr>
    <tr>
        <td>Amarelo</td>
        <td>33/43</td>
        <td>0x3</td>
    </tr>
    <tr>
        <td>Azul</td>
        <td>34/44</td>
        <td>0x4</td>
    </tr>
    <tr>
        <td>Magenta</td>
        <td>35/45</td>
        <td>0x5</td>
    </tr>
    <tr>
        <td>Ciano</td>
        <td>36/46</td>
        <td>0x6</td>
    </tr>
    <tr>
        <td>Branco</td>
        <td>37/47</td>
        <td>0x7</td>
    </tr>
</table>

## VM Features

### Special params

Você pode especificar alguns parâmetros
especias para a VM, os parâmetros são
digitados depois do programa.
`lc3-vm [program] [--params]...`

#### --help

Este parâmetro é especial, é diferente dos
outros parâmetros, não altera nada na
execução da VM, não executa nem mesmo
um programa, --help´é digitado no lugar
do programa e imprime uma pequena descrição
de todos os parâmetros especiais cobertos
nesta seção.

`lc3-vm --help`

#### --runoff

Este parâmetro define o deslocamento do valor inicial do PC.
O valor inicial do PC é 0x3000 + runoff(0x), por padrão runoff
é zero, este parâmetro o altera.

`lc3-vm program.o --runoff A1`
isso altera o valor inicial do PC
para 0x30A1

#### --loadoff

Este parâmetro define o deslocamento
da origem de carregamento do programa.
Os programas são escritos começando em
0x3000 + loadoff(0x), por padrão loadoff é zero,
este parâmetro o altera.

`lc3-vm program.o --loadoff FF`
isso altera a origem do
carregamento do programa
para 0x30FF

! AVISO ! : o runoff não muda com o loadoff, preste atenção onde
seu programa começa

#### --memrep

Este parâmetro cria dois
logs da memória, um antes da
execução do programa e outro
após a execução do programa.

`lc3-vm program.o --memrep`
isso gerará estes dois arquivos
.\\
 | lc3-vm.exe
 | memory\_reports\_[DAY-MONTH-YEAR]\_[HOUR-MIN-SEC]\_final.log
 | memory\_reports\_[DAY-MONTH-YEAR]\_[HOUR-MIN-SEC]\_init.log
