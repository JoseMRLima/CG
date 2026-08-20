# TP CG 2025/2026

Trabalho Prático de **Computação Gráfica** 2025/2026.

**Nota:** 18

## Descrição

Este projeto consiste no desenvolvimento de um **motor gráfico 3D baseado em Scene Graphs**, capaz de processar ficheiros de configuração XML para gerar e visualizar cenas complexas. O trabalho divide-se em duas aplicações principais: um **Generator**, responsável pelo cálculo geométrico e exportação de primitivas gráficas, e um **Engine**, que interpreta a estrutura da cena, gere a câmara e renderiza os modelos.

- **Fase 1** Implementação do gerador de formas básicas (plano, caixa, esfera e cone) e de um motor de visualização capaz de ler os vértices exportados e configurar a câmara através de XML.
- **Fase 2**: Evolução do motor para suportar estruturas hierárquicas (árvores) de grupos, permitindo a aplicação de transformações de translação, rotação e escala acumuladas entre pais e filhos.
- **Fase 3**: Introdução de animações baseadas em curvas de Catmull-Rom, geração de modelos via patches de Bezier e otimização da renderização através da utilização de Vertex Buffer Objects (VBOs).
- **Fase 4**: Finalização do motor com a implementação de iluminação (fontes de luz pontuais, direcionais e focais), aplicação de texturas e suporte para componentes de cor (difusa, ambiente, especular e emissiva).

## Instruções

> Os comandos apresentados devem ser executados nas diretorias das fases (e.g. `home/user/CG-TP-2526/phase1 $ ...`).

Executar as configurações do CMake (**apenas uma vez**):

```shell
cmake -S . -B build
```

**Compilar** o código fonte:

```shell
cmake --build build
```

Gerar **documentação**:

```shell
cmake --build build --target doc
```

Os ficheiros executáveis encontram-se na pasta `build/bin`, por exemplo:

```shell
./build/bin/generator ...
```

## Grupo

Constituintes do grupo de trabalho:

| Nome                      | Número  |
| ------------------------- | ------- |
| Eduardo Freitas Fernandes | a106919 |
| Gonçalo Rodrigues Ribeiro | a106842 |
| José Mário Raimundo Lima  | a106888 |
