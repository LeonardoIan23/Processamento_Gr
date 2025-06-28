# Jogo Isométrico - Trabalho de Processamento Gráfico

Trabalho desenvolvido para a disciplina de **Processamento Gráfico**.

## 👥 Integrantes do Grupo

- Leonardo Ian de Oliveira
- Gustavo Scheffel

## 🚀 Como executar 

Este programa foi feito em **linguagem C++ (padrão C++17)** e utiliza **CMake** para gerir a compilação e as dependências.

### Pré-requisitos
- **CMake** (versão 3.11 ou superior)
- Um **compilador C++** compatível (como MinGW no Windows, GCC no Linux, ou Clang no macOS)

> **Nota:** Não é necessário instalar as bibliotecas (GLFW, GLM) manualmente. O CMake irá descarregá-las e configurá-las automaticamente.

### Passos para Compilação
No seu computador, abra um terminal na pasta raiz do projeto e siga os passos:

1.  **Crie e aceda a uma pasta de build:**
    ```bash
    mkdir build
    cd build
    ```
2.  **Configure o projeto com CMake:**
    *(Se estiver no Windows com MinGW, use o primeiro comando. Para outros sistemas, o segundo deve ser suficiente)*
    ```bash
    # Exemplo para Windows com MinGW
    cmake .. -G "MinGW Makefiles"

    # Exemplo para Linux/macOS
    cmake ..
    ```
3.  **Compile o código:**
    ```bash
    cmake --build .
    ```
4.  **Execute o programa:**
    O executável será criado dentro da pasta `build`. Para executar, use:
    ```bash
    # No Windows
    ./TrabalhoGB_Isometrico.exe

    # No Linux ou macOS
    ./TrabalhoGB_Isometrico
    ```

## 💡 Sobre o Programa

Este programa é um protótipo de um jogo 2D com visão isométrica. O jogador controla um personagem animado em um mapa baseado em tiles, com o objetivo de coletar todos os itens (moedas) enquanto desvia de perigos como lava e água. O jogo inclui mecânicas como um mapa de alturas (que impede quedas altas), tiles que mudam de aparência ao serem pisados, e uma câmara dinâmica que segue o jogador. Toda a configuração do mapa é carregada a partir de um ficheiro de texto, permitindo a fácil modificação dos níveis.

## 📌 Observações

- O código-fonte principal está na pasta `src/TrabaslhoGB/`.
- Os recursos do jogo (mapa, texturas) também se encontram nessa pasta e são copiados automaticamente para a pasta `build` durante a compilação.
- O jogo utiliza as seguintes bibliotecas, geridas pelo `CMakeLists.txt`: **GLFW**, **GLAD**, **GLM** e **stb_image**.



## 📚 Referências 

(Aqui você pode colocar links para tutoriais ou documentação que foram úteis.)
- [Documentação do OpenGL](https://www.opengl.org/documentation/)
- [Documentação do learnopengl.com](https://learnopengl.com/)
- [Guia básico de Markdown no GitHub](https://docs.github.com/pt/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)