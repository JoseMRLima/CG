// main.typ
#import "lib.typ": conf
#show: conf.with(
  titulo: "Trabalho Prático: Fase 4",
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

Esta fase acrescenta ao sistema desenvolvido nas fases anteriores suporte a iluminação, materiais e texturas. O _generator_ foi atualizado para calcular e exportar normais e coordenadas de textura para todas as primitivas. O _engine_ passou a configurar fontes de luz OpenGL, a aplicar propriedades de material por modelo e a carregar imagens para texturas através da biblioteca _stb\_image_. O formato do ficheiro `.3d` foi estendido para conter, por vértice, posição, normal e coordenadas UV. As funcionalidades das fases anteriores (VBOs/IBOs, animações, _Bézier patches_) mantêm-se inalteradas.

= Organização do Código

A estrutura do projeto mantém a divisão em _packages_ introduzida na fase 3:

- `generator/`: geração de primitivas geométricas, agora com normais e coordenadas UV.
- `engine/`: _parsing_ do XML, gestão da cena e ciclo principal.
- `lib/core/`: estruturas de dados partilhadas (`Model`, `Point`, `Vector`). A classe `Model` foi estendida com as estruturas `Material` e `UV`, e com lógica de carregamento de texturas.
- `lib/stb_image/`: inclusão da biblioteca _stb\_image_ para leitura de ficheiros de imagem.
- `lib/xml/`: dependência _tinyxml2_.


= Arquitetura do Sistema

#align(center)[
  #figure(
    image("imgs/Arquitetura.jpg", width: 65%),
    caption: [Arquitetura do Sistema]
  )<arquitetura-uml>
]

A classe `World` passa a manter uma lista de instâncias de `Light` e a chamar `Light::enableLighting()` durante a inicialização e `Light::setup(i)` para cada luz antes de desenhar a cena. A classe `Model` foi estendida com:

- `std::vector<UV> uvs`: coordenadas de textura por vértice;
- `Material material`: propriedades de cor e brilho;
- `unsigned int _texture`: identificador da textura OpenGL;
- método `loadTexture()`: carrega a imagem para o _GPU_ via _stb\_image_.

O `SceneParser` foi atualizado para ler os novos elementos `<lights>`, `<texture>` e `<color>` do XML, populando os campos correspondentes em `World` e `Model`.

O VBO passou a armazenar, por vértice, oito valores de ponto flutuante: três de posição, três de normal e dois de coordenada de textura, com a organização descrita na @vbo-layout.

#align(center)[
  #figure(
    table(
      columns: (auto, auto, auto, auto, auto, auto, auto, auto),
      align: center,
      table.header([px], [py], [pz], [nx], [ny], [nz], [u], [v]),
      [`float`], [`float`], [`float`], [`float`], [`float`], [`float`], [`float`], [`float`],
    ),
    caption: [Organização de um vértice no VBO (8 × `float`, 32 bytes)]
  )<vbo-layout>
]

= Ficheiro _.3d_

O formato `.3d` mantém a estrutura da fase anterior mas cada linha de vértice passa a conter oito valores:

```
<n_vertices>
px py pz  nx ny nz  u v
...
```

onde $(p_x, p_y, p_z)$ é a posição, $(n_x, n_y, n_z)$ é a normal unitária e $(u, v)$ são as coordenadas de textura em $[0,1]^2$. O número inteiro na primeira linha indica o total de vértices.

A classe `Model` lê estes oito valores por vértice em `loadFromFile()` e constrói, em `setup()`, o VBO indexado eliminando vértices duplicados com base numa chave que compara os oito campos simultaneamente. Dois vértices são considerados distintos se diferirem em qualquer um dos campos, o que preserva descontinuidades de normal e de textura.

= _Generator_

== Plano

Um plano é subdividido numa grelha regular de $"divisions" times "divisions"$ quadriláteros, cada um dividido em dois triângulos. Como a superfície é plana, todos os vértices partilham a mesma normal, calculada uma única vez como o produto externo normalizado dos dois vetores diretores $bold(u)$ e $bold(v)$:

$ hat(bold(n)) = frac(bold(u) times bold(v), |bold(u) times bold(v)|) $

As coordenadas de textura são obtidas normalizando a distância ao longo de cada eixo para $[0, 1]$. Para uma célula na posição $(i, j)$ da grelha, com passo $"increment" = "length" \/ "divisions"$:

$
  u = frac(i times "increment", "length"), quad
  v = frac(j times "increment", "length")
$

Os quatro cantos de cada célula recebem assim as coordenadas UV $(u, v)$, $(u + 1\/"divisions", v)$, $(u, v + 1\/"divisions")$ e $(u + 1\/"divisions", v + 1\/"divisions")$.


== Cubo

Um cubo é construído a partir de seis faces planas independentes. Cada face reutiliza a construção do plano com vetores diretores $bold(u)$ e $bold(v)$ ortogonais, escolhidos de modo a que $bold(u) times bold(v)$ aponte sempre para o exterior. O cálculo das normais e das coordenadas UV é portanto idêntico ao do plano.

== Esfera

A esfera é parametrizada pelo ângulo polar $theta in [0, pi]$ (associado aos _stacks_) e pelo ângulo azimutal $phi in [0, 2pi]$ (associado aos _slices_):

$
  bold(p)(theta, phi) = "radius" dot
  (sin theta sin phi, quad cos theta, quad sin theta cos phi)
$

Como cada ponto da superfície se encontra à distância $"radius"$ da origem, a
normal unitária exterior é simplesmente o vetor posição normalizado:

$ hat(bold(n)) = frac(bold(p), |bold(p)|) = frac(bold(p), "radius") $

Não é necessário calcular nenhum produto externo.

Para as coordenadas de textura os ângulos são mapeados linearmente para $[0, 1]^2$ através de uma projeção equiretangular (latitude-longitude):

$ u = frac(phi, 2pi), quad v = frac(theta, pi) $

== Cone

O cone tem duas regiões distintas: a base circular e a superfície lateral.

=== Base

A base está orientada para baixo, pelo que a sua normal é constante:

$ hat(bold(n))_"base" = (0, -1, 0) $

As coordenadas UV colocam o centro da base em $(0.5, 0.5)$ e mapeiam os pontos do bordo radialmente num disco:

$ u = 0.5 + 0.5 cos theta, quad v = 0.5 + 0.5 sin theta $

=== Superfície Lateral

Um ponto no _stack_ $i in \{0, dots, "stacks" - 1\}$ e no _slice_ $j$ é parametrizado pela fração de altura $t_i = i \/ "stacks"$:

$
  "rBottom"(i) = (1 - t_i) dot "radius", quad
  h(i) = t_i dot "height", quad
  theta_j = frac(2pi j, "slices")
$

$ bold(p)(i, j) = ("rBottom"(i) cos theta_j, quad h(i), quad "rBottom"(i) sin theta_j) $

*Normais.* A normal lateral tem de estar inclinada para o exterior de acordo com o ângulo de semi-abertura do cone. Com comprimento de geratriz $L = sqrt("radius"^2 + "height"^2)$, a normal unitária exata no ângulo $theta_j$ é:

$
  hat(bold(n)) = frac(1, L)
  ("height" sin theta_j, quad "radius", quad "height" cos theta_j)
$

Na implementação, isto é expresso proporcionalmente como

$
  bold(n)_"raw" =
  (sin theta_j, quad "rBottom"(i) \/ "height", quad cos theta_j)
$

e depois normalizado, obtendo a mesma direção.

*Coordenadas de Textura.* A superfície lateral é desdobrada cilindricamente:

$ u = frac(theta_j, 2pi), quad v = t_i = frac(i, "stacks") $

== Cilindro

Um cilindro tem três regiões: duas tampas planas e uma banda lateral.

=== Tampas

Ambas as tampas têm normais constantes alinhadas com o eixo $y$:

$
  hat(bold(n))_"top" = (0, 1, 0), quad
  hat(bold(n))_"bot" = (0, -1, 0)
$

As coordenadas UV mapeiam o bordo num disco, de forma idêntica à base do cone:

$ u = 0.5 + 0.5 cos alpha, quad v = 0.5 + 0.5 sin alpha $

onde $alpha = i dot 2pi \/ "slices"$ e $"nextAlpha" = (i+1) dot 2pi \/ "slices"$.

=== Superfície Lateral

Como o cilindro não tem inclinação, a normal exterior no azimute $alpha$ é puramente radial:

$ hat(bold(n)) = (cos alpha, quad 0, quad sin alpha) $

O desdobramento UV atribui $v = 0$ ao bordo inferior e $v = 1$ ao bordo superior:

$ u = frac(alpha, 2pi), quad v in \{0, 1\} $

== Anel

O anel é parametrizado pelo ângulo maior $theta in [0, 2pi]$ (_rings_) e pelo ângulo do tubo $phi in [0, 2pi]$ (_sides_), com raio maior $"majorRadius"$ e raio do tubo $"minorRadius"$:

$
  bold(p)(theta, phi) = ( \
    &("majorRadius" + "minorRadius" cos phi) cos theta, \
    &"minorRadius" sin phi, \
    &("majorRadius" + "minorRadius" cos phi) sin theta \
  )
$

A normal unitária exterior aponta para fora do eixo do tubo:

$
  hat(bold(n)) = (cos phi cos theta, quad sin phi, quad cos phi sin theta)
$

Este vetor é já unitário, dado que $cos^2 phi + sin^2 phi = 1$.

Para as coordenadas de textura, ambos os ângulos são mapeados linearmente para $[0, 1]^2$:

$ u = frac(theta, 2pi), quad v = frac(phi, 2pi) $

== _Bezier Patches_

Um patch de Bézier é definido por 16 pontos de controlo $bold(P)_(i j)$, com $i, j in \{0, 1, 2, 3\}$, e parâmetros $u, v in [0, 1]$.

$
  bold(S)(u, v)
  = sum_(i=0)^(3) sum_(j=0)^(3) B_i (u) B_j (v) bold(P)_(i j)
$

onde $B_k (t)$ são os polinómios de Bernstein cúbicos:

$
  B_0 (t) &= (1 - t)^3 \
  B_1 (t) &= 3t(1 - t)^2 \
  B_2 (t) &= 3t^2 (1 - t) \
  B_3 (t) &= t^3
$

A normal é obtida pelo produto externo dos dois vetores tangentes parciais. O tangente na direção $u$ é:

$
  bold(T)_u = frac(partial bold(S), partial u)
  = sum_(i=0)^(3) sum_(j=0)^(3) B'_i (u) B_j (v) bold(P)_(i j)
$

e na direção $v$:

$
  bold(T)_v = frac(partial bold(S), partial v)
  = sum_(i=0)^(3) sum_(j=0)^(3) B_i (u) B'_j (v) bold(P)_(i j)
$

As derivadas dos polinómios de Bernstein são:

$
  B'_0 (t) &= -3(1 - t)^2 \
  B'_1 (t) &= 3(1 - t)^2 - 6t(1 - t) \
  B'_2 (t) &= 6t(1 - t) - 3t^2 \
  B'_3 (t) &= 3t^2
$

A normal unitária exterior é então:

$
  hat(bold(n)) = frac(bold(T)_v times bold(T)_u,
    |bold(T)_v times bold(T)_u|)
$

Quando o _patch_ degenera — i.e., quando $|bold(T)_v times bold(T)_u| < 10^(-6)$, utiliza-se a normal de recurso $(0, 1, 0)$.

As coordenadas paramétricas são usadas diretamente como coordenadas UV. Para a célula no índice de tesselação $(u_i, v_i)$ com nível $"tessellation"$:

$
  u = frac(u_i, "tessellation"), quad v = frac(v_i, "tessellation")
$

A textura é assim esticada uniformemente sobre cada _patch_, sem qualquer correção de distorção adicional.

= _Engine_

== Iluminação

O sistema de iluminação usa o modelo de iluminação de Phong disponível no _pipeline_ fixo do OpenGL. São suportados três tipos de fonte de luz, configuráveis via XML:

- *Pontual* (`point`): emite luz em todas as direções a partir de uma posição. Configurada com `GL_POSITION` com componente $w = 1$.
- *Direcional* (`directional`): simula uma fonte à distância infinita, com raios paralelos numa direção fixa. Configurada com `GL_POSITION` com componente $w = 0$.
- *Foco* (`spot`): emite um cone de luz com ângulo de abertura `cutoff` e concentração `exponent`. Configurada com `GL_SPOT_DIRECTION`, `GL_SPOT_CUTOFF` e `GL_SPOT_EXPONENT`.

A inicialização da iluminação ocorre uma única vez em `World::setup()`, que chama `Light::enableLighting()`. Este método ativa `GL_LIGHTING`, `GL_NORMALIZE` (para renormalizar as normais após escalonamento) e define `GL_LIGHT_MODEL_TWO_SIDE` para iluminar ambas as faces dos triângulos.

Em cada _frame_, antes de qualquer transformação de modelo, `World::draw()` itera sobre as luzes e chama `Light::setup(i)` para cada uma, registando a posição e direção no espaço do mundo.

O XML aceita o seguinte formato para declarar luzes:

```xml
<lights>
  <light type="point"       posx="0"  posy="0"  posz="0" />
  <light type="directional" dirx="1"  diry="0"  dirz="0" />
  <light type="spot"        posx="0"  posy="5"  posz="0"
                            dirx="0"  diry="-1" dirz="0"
                            cutoff="30" exponent="2" />
</lights>
```

== Materiais

Cada modelo pode ter um conjunto de propriedades de material definidas no XML através do elemento `<color>`. As componentes suportadas são:

- *Difusa* (`diffuse`): cor principal, modulada pelo ângulo de incidência da luz.
- *Ambiente* (`ambient`): contribuição constante, independente da orientação.
- *Especular* (`specular`): reflexo brilhante, dependente do ângulo de visualização.
- *Emissiva* (`emissive`): luz própria do objeto, independente das fontes externas.
- *Brilho* (`shininess`): expoente que controla a concentração do reflexo especular.

Os valores RGB são dados em $[0, 255]$ no XML e convertidos para $[0, 1]$ pelo _parser_. Em `Model::draw()`, as propriedades são aplicadas via `glMaterialfv` antes de cada `glDrawElements`, afetando apenas o modelo a que pertencem.

```xml
<color>
  <diffuse  R="169" G="169" B="169" />
  <ambient  R="30"  G="30"  B="30"  />
  <specular R="80"  G="80"  B="80"  />
  <emissive R="0"   G="0"   B="0"   />
  <shininess value="20" />
</color>
```

== Texturas

O carregamento de texturas é feito com a biblioteca _stb\_image_, integrada diretamente no projeto em `lib/stb_image/`. O método `Model::loadTexture()` é chamado dentro de `Model::setup()`, após o contexto OpenGL estar disponível, e executa os seguintes passos:

1. Lê a imagem com `stbi_load`, forçando três canais (RGB) e invertendo verticalmente para compensar a diferença de origem entre _stb\_image_ e OpenGL.
2. Cria um objeto de textura com `glGenTextures` e carrega os dados para a GPU via `glTexImage2D`.
3. Gera _mipmaps_ automáticos com `glGenerateMipmap` e configura filtragem linear (`GL_LINEAR`) e repetição (`GL_REPEAT`).

Durante o desenho, se o modelo tiver textura, `Model::draw()` ativa `GL_TEXTURE_2D`, associa a textura com `glBindTexture` e define o modo `GL_MODULATE`, que combina a cor da textura com a contribuição do material. O VBO inclui as coordenadas UV e o _pointer_ `GL_TEXTURE_COORD_ARRAY` aponta para o deslocamento correto dentro do mesmo _buffer_ interleaved.

A textura é declarada no XML através de:

```xml
<model file="sphere.3d">
  <texture file="textures/earth.jpg" />
</model>
```

= _Demo Scene_

A cena de demonstração representa o Sistema Solar e aplica todas as funcionalidades introduzidas nesta fase.

*Iluminação.* Uma luz pontual é colocada na origem, representando o Sol. Desta forma, os planetas recebem luz de acordo com a sua orientação face ao Sol, com as faces voltadas para ele mais iluminadas.

*Sol.* Representado por uma esfera escalonada ($times 3$) com componente emissiva ativa (amarelo-laranja), tornando-o visível mesmo sem fontes de luz externas.

*Planetas.* Todos os planetas possuem textura e material configurados individualmente. As propriedades de material foram escolhidas para refletir as características visuais de cada corpo celeste (e.g., Neptuno tem componente especular azul intensa; Mercúrio tem especular reduzida). As órbitas são animadas com `<rotate time="...">`.

*Satélites.* A Lua orbita a Terra, Fobos e Deimos orbitam Marte, e Ganimedes orbita Júpiter. Todos herdam a translação do planeta-pai através da hierarquia de grupos, tendo depois as suas próprias rotações no referencial local.

*Saturno.* Além da textura no planeta, foi adicionado um anel (`saturn_ring.3d`) com rotação estática inclinada e material próprio.

*Bule.* O _teapot_ orbita numa curva de _Catmull-Rom_ tridimensional com `align="true"`, mantendo a orientação alinhada com a tangente da trajetória. Tem textura e material especular elevado (`shininess = 128`).

#align(center)[
  #figure(
    image("imgs/demo_scene.png", width: 80%),
    caption: [Sistema Solar com iluminação e texturas]
  )
]

= Conclusão

Esta fase completou o sistema com iluminação, materiais e texturas. O _generator_ foi atualizado para exportar normais e coordenadas UV para todas as primitivas. O formato _.3d_ passou a incluir esses dados, e o VBO foi reorganizado para os conter de forma _interleaved_. O _engine_ passou a configurar luzes OpenGL (pontuais, direcionais e focos), a aplicar materiais por modelo via `glMaterialfv` e a carregar texturas com _stb\_image_. A _demo scene_ do Sistema Solar demonstra todas estas funcionalidades em conjunto: a luz pontual central ilumina os planetas de forma realista, as texturas individualizam cada corpo celeste, e os materiais controlam o brilho e as reflexões especulares de cada objeto.
