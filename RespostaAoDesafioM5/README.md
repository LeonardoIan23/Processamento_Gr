# Atividade: Controle e Animação de Sprites (Módulo 5)

Trabalho desenvolvido para a disciplina **Processamento Gráfico**.

## 👥 Integrantes do Grupo

- Leonardo Ian de Oliveira

## 🚀 Como Compilar e Executar

Este projeto utiliza **C++** e **CMake** para gerenciar a compilação.

### Pré-requisitos

- Compilador C++ (como o MinGW no Windows, ou g++ no Linux).
- CMake (versão 3.10 ou superior).

### Passos para Compilação

1.  Clone este repositório para o seu computador.
2.  Abra um terminal na pasta raiz do projeto.
3.  Crie uma pasta `build` para a compilação:
    ```bash
    mkdir build
    ```
4.  Entre na pasta `build`:
    ```bash
    cd build
    ```
5.  Execute o CMake para configurar o projeto:
    ```bash
    cmake ..
    ```
6.  Compile o projeto (no Windows com MinGW, o comando geralmente é):
    ```bash
    mingw32-make
    ```
    (No Linux ou macOS, o comando seria apenas `make`).

7.  Após a compilação, o executável (`main.exe` ou `main`) será criado dentro da pasta `build`.

### Como Rodar

1.  No terminal, ainda dentro da pasta `build`, execute o programa:
    ```bash
    ./main.exe
    ```

## 💡 Sobre o Programa

Este programa demonstra a implementação de uma classe `Sprite` em C++ com OpenGL. A aplicação renderiza um personagem animado que pode ser controlado pelo teclado.

- O personagem é animado a partir de um **spritesheet**, com diferentes animações para cada direção.
- O controle é feito pelas teclas **W, A, S, D**.
- O programa implementa um estado "parado" (idle), onde a animação da caminhada cessa quando nenhuma tecla de movimento é pressionada.

## 📌 Observações

- A lógica principal foi organizada e encapsulada na classe `Sprite` (arquivos `Sprite.h` e `Sprite.cpp`). O arquivo `main.cpp` é responsável pelo loop principal e pelo controle do teclado.
- O sprite utilizado (`player.png`) é um 'spritesheet' com 4 direções de animação (4 linhas) e 4 quadros (frames) por animação.
- O código foi desenvolvido com base nos exemplos fornecidos no Módulo 5 e refatorado para usar uma estrutura de Orientação a Objetos, como sugerido na tarefa.

---
Obrigado e espero que goste do trabalho!