// main.typ
#import "lib.typ": conf
#show: conf.with(
  titulo: "Trabalho Prático: Fase 3",
  uc: "Computação Gráfica",
  sigla: "CG",
  grupo: "03",
  ano: "2025/2026",
  alunos: (
    (id: "A106919", nome: "Eduardo Freitas Fernandes"),
    (id: "A106842", nome: "Gonçalo Rodrigues Ribeiro"),
    (id: "A106888", nome: "José Mário Raimundo Lima"),
  ),
)


= Introdução

Esta fase estende o sistema desenvolvido nas fases anteriores com três novas funcionalidades: geração de superfícies por _Bezier patches_, migração do _rendering_ para _Vertex Buffer Objects_ (VBOs) com _Index Buffer Objects_ (IBOs), e suporte a animações temporais, translação ao longo de curvas de _Catmull-Rom_ e rotação contínua.

A estrutura geral do sistema mantém-se: o `SceneParser` carrega o XML, a hierarquia de `Group` organiza a cena, e o `Engine` renderiza _frame_ a _frame_. As alterações concentram-se na classe `Model` (que passa a usar VBOs), na adição de uma nova hierarquia `Animation`, e num novo modo no _Generator_ para _patches_.

= Organização do Código

Nesta terceira fase de desenvolvimento, a estrutura do projeto foi submetida a uma reorganização modular significativa, visando aumentar a escalabilidade e a separação de responsabilidades. Diferente da fase anterior, em que os ficheiros estavam agrupados em pastas genéricas (`src` e `include`), optou-se agora por uma distribuição por _packages_ que refletem as camadas lógicas da aplicação:
- `engine/`: Concentra a lógica de _rendering_ da cena, _parsing_ de ficheiros XML e o ciclo principal.
- `generator/`: Contém as ferramentas de geração de primitivas geométricas e formas.
- `lib/`: Subdividida em `core/`, que contém as estruturas fundamentais de geometria (`Model`, `Point`, `Vector`), e `xml/`, que isola a dependência externa `tinyxml2`.

= Arquitetura do Sistema

O diagrama de classes da @arquitetura-uml reflete a maturidade da arquitetura, mantendo a consistência com as fases anteriores, mas introduzindo outros mecanismos. A estrutura permanece centrada na classe `World`, que atua como contentor raiz para a janela, câmara e a hierarquia de grupos de objetos.

#align(center)[
  #figure(
    image("imgs/Arquitetura.jpg", width: 60%),
    caption: [Arquitetura do Sistema]
  )<arquitetura-uml>
]

A principal evolução arquitetural desde a segunda fase reside na introdução da hierarquia `Animation`. Enquanto que na fase anterior as transformações geométricas eram estáticas, a arquitetura agora suporta transformações dependentes do tempo. Através da interface `Animation` e das suas especializações (`TimedTranslation` e `TimedRotation`), o motor é capaz de interpolar estados e atualizar a matriz de transformação de cada `Group` de forma cíclica.

As demais componentes, como a representação de vértices e vetores na camada _core_ e a geração de malhas via `Generator`, mantêm-se como a base sólida sobre a qual estas novas funcionalidades de animação foram construídas.

= _Bezier Patches_

Uma superfície _Bezier_ cúbica é definida por uma grelha de $4 times 4 = 16$ pontos de controlo. Dado um par de parâmetros $(u, v) in [0,1]^2$, o ponto na superfície calcula-se como:

$
P(u, v) = sum_(i=0)^(3) sum_(j=0)^(3) B_i(u) dot B_j(v) dot P_(i j)
$

onde $B_i$ são os polinómios de _Bernstein_ de grau 3:

$
B_0(t) = (1-t)^3, quad B_1(t) = 3t(1-t)^2, quad B_2(t) = 3t^2(1-t), quad B_3(t) = t^3
$

O `Generator` recebe um ficheiro `.patch` com a topologia (índices dos 16 pontos de controlo por _patch_) e as coordenadas dos pontos de controlo. Para cada _patch_, divide o domínio $(u,v)$ em `tessellation` $times$ `tessellation` células e avalia $P(u,v)$ nos quatro cantos de cada célula, gerando dois triângulos por célula.

A invocação no _Generator_ é:

```
./build/bin/generator patch <ficheiro.patch> <tessellation> <output.3d>
```

= _Vertex Buffer Objects_

Nas fases anteriores, os vértices eram enviados ao OpenGL triângulo a triângulo via `glBegin`/`glEnd` (_immediate mode_). Nesta fase, os dados são carregados para memória da GPU numa única operação e reutilizados em cada _frame_.

A classe `Model` passa a manter dois _buffers_ na GPU:

- *VBO* (_Vertex Buffer Object_): armazena as coordenadas únicas dos vértices como `float`.
- *IBO* (_Index Buffer Object_): armazena, para cada triângulo, os índices dos três vértices no VBO.

O método `setup()` é chamado uma vez após o carregamento da cena e envia os dados para a GPU via `glBufferData`. O método `draw()` limita-se a ligar os _buffers_ e emitir um único `glDrawElements`, sem transferir dados entre CPU e GPU.

== _Index Buffer Objects_

A função `generateBuffers` (em `utils.cpp`) constrói o VBO e o IBO a partir da lista de vértices do modelo. Usa um `std::map<Point, unsigned int>` para detetar pontos duplicados: a primeira ocorrência de um ponto é adicionada ao VBO e recebe um índice novo; ocorrências repetidas reutilizam o índice existente. O resultado é um VBO compacto (apenas vértices únicos) e um IBO que descreve a triangulação por referência a esses vértices.

= Animações

As transformações geométricas estáticas foram expandidas para suportar comportamentos dinâmicos baseados no tempo real. O tempo decorrido, obtido através da função `glutGet(GLUT_ELAPSED_TIME)`, é convertido para segundos e propagado através do grafo de cena para atualizar as transformações dinâmicas.

== Translação

A translação dinâmica permite que um modelo percorra uma trajetória definida por um conjunto de pontos de controlo que formam uma curva de _Catmull-Rom_. Esta curva garante continuidade $C^1$, passando por todos os pontos de controlo intermédios, exceto o primeiro e o último.

O cálculo da posição $P(t)$ num segmento da curva, onde $t in [0, 1[$, é realizado através da seguinte equação matricial:

$
P(t) = 1/2 mat(t^3, t^2, t, 1) mat(-1, 3, -3, 1; 2, -5, 4, -1; -1, 0, 1, 0; 0, 2, 0, 0) vec(P_(i-1), P_i, P_(i+1), P_(i+2))
$

Quando o atributo `align` é ativado, o modelo é orientado segundo o vetor tangente à curva, $P'(t)$, calculado pela derivada da posição em ordem a $t$:

$
P'(t) = 1/2 mat(3t^2, 2t, 1, 0) mat(-1, 3, -3, 1; 2, -5, 4, -1; -1, 0, 1, 0; 0, 2, 0, 0) vec(P_(i-1), P_i, P_(i+1), P_(i+2))
$

A orientação é completada através do cálculo de uma base ortonormada ($X_"local"$, $Y_"local"$, $Z_"local"$) recorrendo a produtos vetoriais entre a tangente (eixo $X$) e um vetor auxiliar de verticalidade ($0, 1, 0$).

A lógica de translação foi encapsulada na classe `TimedTranslation`. Esta entidade armazena os pontos de controlo e o período temporal, delegando o cálculo da curva para funções utilitárias que implementam a multiplicação matricial de _Catmull-Rom_. O alinhamento é resolvido dinamicamente: a tangente calculada é normalizada para definir o eixo $X$ local, e os restantes eixos são deduzidos por produtos vetoriais, resultando numa matriz de rotação $4 times 4$ injetada diretamente no _pipeline_ gráfico.

== Rotação

A rotação temporal define a velocidade de rotação de um objeto em torno de um eixo, especificada pelo tempo necessário para completar uma volta completa. O ângulo de rotação $alpha$ num instante $Delta t$ é calculado proporcionalmente ao ciclo definido:

$
alpha = ((Delta t) / "tempo") times 360 degree
$

Esta funcionalidade é gerida pela classe `TimedRotation`, que converte o tempo absoluto da aplicação num ângulo acumulado para a função de rotação do _OpenGL_. Esta abordagem garante que o movimento seja fluido e independente da taxa de atualização (_frame rate_) da aplicação.

== Integração no Grafo de Cena

A ligação entre o tempo global e as transformações individuais é feita através da classe `Group`. Durante a renderização, cada grupo executa uma fase de atualização de animações antes de desenhar os seus modelos:

1. O tempo atual é capturado e passado às instâncias de animação.
2. Cada animação (`TimedRotation` ou `TimedTranslation`) calcula o seu estado atual e aplica as transformações à matriz de visualização.
3. Como estas operações ocorrem dentro do contexto de matriz do grupo, todos os subgrupos e modelos descendentes herdam automaticamente o movimento animado, respeitando a hierarquia da cena.

= _Demo Scene_

Como prova de conceito das funcionalidades implementadas, foi desenvolvida uma cena representativa do Sistema Solar, focada na aplicação prática de hierarquias de transformação e animações temporais.

- *Hierarquia de Transformações:* A cena utiliza uma estrutura de grafos de cena onde os planetas e satélites são definidos como grupos dependentes. Esta organização permite que as luas herdem as transformações de translação e rotação dos seus respetivos grupos, mantendo as suas próprias órbitas.
- *Dinâmica Orbital:* As órbitas foram modeladas através da aplicação de rotações baseadas no tempo, permitindo velocidades angulares distintas para cada corpo celeste, simulando a mecânica orbital de forma contínua.
- *Geometria Complexa e Curvas de Bézier:* A cena inclui um bule, utilizado para demonstrar a capacidade do motor em processar modelos gerados a partir de *Bézier Patches*. Este modelo foi integrado numa trajetória 3D dinâmica utilizando uma curva de Catmull-Rom, com variação nos três eixos cartesianos e alinhamento automático.

#align(center)[
  #figure(
    image("imgs/demo_scene.png", width: 60%),
    caption: [_Demo Scene_]
  )
]

= Conclusão

Esta fase introduziu três extensões ao sistema. Os VBOs eliminam a transferência de dados por _frame_, delegando ao OpenGL a gestão eficiente da geometria na GPU. Os _Bézier patches_ permitem gerar superfícies curvas suaves a partir de ficheiros de controlo padrão, como o _teapot_. As animações temporais, curva de Catmull-Rom e rotação contínua, tornam a cena dinâmica, com objetos a orbitar e a rodar de forma autónoma e configurável pelo XML.
