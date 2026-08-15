# Solar System - Guia de Utilização

> ⚠️ A Makefile foi testada apenas em **Linux e macOS**.

## Pré-requisitos

- CMake ≥ 3.15
- Compilador C++17 (GCC ou Clang)
- OpenGL + GLUT instalados

## Comandos disponíveis

### 1. Setup

Cria a pasta `build` e configura o CMake. **Só é necessário correr uma vez.**

```bash
make setup
```

### 2. Compilar

Compila o `engine` e o `generator`.

```bash
make compile
```

> `make compile` é também o comportamento por omissão de `make` (sem argumentos).

### 3. Testes

Cada teste gera os ficheiros `.3d` necessários e abre o engine com a cena correspondente.

| Comando      | Descrição                                                      |
| ------------ | -------------------------------------------------------------- |
| `make test1` | Cubo amarelo com iluminação                                    |
| `make test2` | Cubo laranja com iluminação                                    |
| `make test3` | Teapot, cone, esfera e cubo assentados num plano               |
| `make test4` | Teapot, cone, esfera e cubo assentados num plano               |
| `make test5` | Teapot, cone, esfera e cubo assentados num plano               |
| `make test6` | Teapot, cone, esfera e cubo assentados num plano, com texturas |

### 4. Demo - Sistema Solar

Gera os modelos do sistema solar e executa a cena completa.

```bash
make demo
```
