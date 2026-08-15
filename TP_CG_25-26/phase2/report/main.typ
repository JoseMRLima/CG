// main.typ
#import "lib.typ": conf

#show: conf.with(
  titulo: "Trabalho Prático: Fase 2",
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

O presente relatório descreve o desenvolvimento da segunda fase do projeto prático de Computação Gráfica. Esta fase tem como objetivo principal a extensão do sistema desenvolvido na fase anterior, introduzindo suporte a transformações geométricas hierárquicas e a organização da cena em forma de grafo de cena (_scene graph_).

Relativamente à fase 1, o _Engine_ foi reestruturado para suportar grupos aninhados, onde cada grupo pode conter um conjunto de transformações geométricas, modelos e subgrupos. Esta hierarquia permite a criação de cenas complexas com relações de dependência entre objetos, como planetas com luas que herdam o posicionamento do planeta pai.

Para concretizar esta extensão, foram introduzidas duas novas componentes: a classe `SceneParser`, responsável pelo carregamento e interpretação do ficheiro XML de configuração, e a hierarquia de classes `Transformation`, que abstrai as operações de translação, rotação e escala. A classe `Group` foi igualmente reformulada para acomodar transformações e subgrupos.

A _demo scene_ obrigatória desta fase é um modelo estático do sistema solar, incluindo o Sol, os oito planetas e algumas luas, organizados numa hierarquia de grupos que reflete as relações de posicionamento relativo entre os corpos celestes.


= Arquitetura do Sistema

Nesta fase, a arquitetura do sistema foi reorganizada face à fase anterior. A principal alteração estrutural foi a separação da lógica de _parsing_ XML da classe `World`, que na fase 1 acumulava responsabilidades tanto de armazenamento da cena como de leitura do ficheiro de configuração.

Esta separação originou uma nova classe, `SceneParser`, que centraliza toda a lógica de interpretação do XML. A classe `World` passou a ser uma estrutura de dados pura, limitando-se a agregar a janela (`Window`), a câmara (`Camera`) e o grupo raiz da cena (`Group`).

// NOTA: meter aqui o diagrama de arquitetura da fase 2 (semelhante ao da fase 1 mas com SceneParser destacado e Group com subgrupos/transformações)

O diagrama acima ilustra como o `SceneParser` medeia a relação entre o ficheiro XML e a estrutura de dados em memória. Internamente, o `SceneParser` utiliza a biblioteca `tinyxml2` para navegar na árvore XML, delegando a construção dos objetos às respetivas classes.

== Classe `SceneParser`

A classe `SceneParser` expõe um único método público estático:

```
loadWorld(filename : string, world : World) : bool
```

Este método coordena o carregamento completo da cena, invocando internamente quatro funções auxiliares privadas: `parseWindow`, `parseCamera`, `parseModels` e `parseGroup`. Cada uma destas funções recebe o elemento XML correspondente e preenche a estrutura de dados adequada.

A função `parseGroup` é recursiva: ao encontrar elementos `<group>` filhos dentro de um grupo, invoca-se a si própria para construir o subgrupo, que é depois adicionado ao grupo pai. Esta recursividade natural reflete a estrutura em árvore do XML.

A função `parseTransform` itera pelos filhos do elemento `<transform>`, identificando cada transformação pelo seu nome (`translate`, `rotate` ou `scale`) e instanciando o objeto concreto correspondente, que é adicionado ao grupo através de um `unique_ptr<Transformation>`.

== Hierarquia `Transformation`

Para representar as transformações geométricas de forma extensível, foi definida uma hierarquia de classes com base numa classe abstrata `Transformation`:

#align(center)[
  #figure(
    image("imgs/hierarquia_transformation.png", width: 80%),
    caption: [Hierarquia `Transformation`]
  )
]

A classe base declara um único método virtual puro `apply()`, que cada subclasse implementa recorrendo diretamente às funções da _pipeline_ OpenGL:

- `Translate::apply()` invoca `glTranslatef(x, y, z)`
- `Rotate::apply()` invoca `glRotatef(angle, x, y, z)`
- `Scale::apply()` invoca `glScalef(x, y, z)`

Esta abordagem permite que o grupo itere sobre uma lista heterogénea de transformações e as aplique em sequência, sem necessidade de conhecer o tipo concreto de cada uma. A ordem de inserção na lista é respeitada, o que é determinante para o resultado final, dado que as transformações geométricas não são, em geral, comutativas.

== Classe `Group`

A classe `Group` foi reformulada para acomodar três coleções distintas:

- lista de transformações
- lista de modelos
- lista de sub-grupos

#align(center)[
  #figure(
    image("imgs/classe_group.png", width: 80%),
    caption: [Estrutura da classe `Group`]
  )
]
As transformações são guardadas como `std::vector<std::unique_ptr<Transformation>>`, o que permite armazenar objetos polimórficos de forma eficiente e com gestão automática de memória. Os subgrupos são armazenados como `std::vector<Group>`, suportando aninhamento arbitrário.

O método `draw()` do grupo segue a seguinte sequência:

+ Chamar `glPushMatrix()` para salvar o estado da matriz de transformação atual.
+ Aplicar, em ordem, todas as transformações do grupo (`t->apply()` para cada `t`).
+ Desenhar todos os modelos do grupo.
+ Recursivamente chamar `draw()` em cada subgrupo filho.
+ Chamar `glPopMatrix()` para restaurar a matriz ao estado anterior.

Este mecanismo de _push/pop_ garante que as transformações de um grupo não se propagam para os seus irmãos, mas propagam-se corretamente para os seus filhos, implementando assim a herança hierárquica de transformações.


= Transformações Geométricas

Nesta secção descrevem-se as três transformações geométricas suportadas, a sua representação matemática e o seu efeito na _pipeline_ de renderização do OpenGL.

== Translação

A translação desloca um objeto no espaço tridimensional ao longo dos eixos $x$, $y$ e $z$. Matematicamente, corresponde à adição de um vetor de deslocamento $bold(t) = (t_x, t_y, t_z)$ às coordenadas de cada vértice:

$
P' = P + bold(t) = (x + t_x, quad y + t_y, quad z + t_z)
$

Em termos de álgebra linear com coordenadas homogéneas, a translação é representada pela matriz $4 times 4$:

$
T(t_x, t_y, t_z) = mat(
  1, 0, 0, t_x;
  0, 1, 0, t_y;
  0, 0, 1, t_z;
  0, 0, 0, 1
)
$

No contexto da @demo_scene (Demo Scene), a translação é frequentemente usada para posicionar objetos relativamente ao seu grupo pai. Por exemplo, um planeta é movido para a sua órbita em relação ao Sol, que está na origem do grupo raiz.

== Rotação

A rotação de ângulo $alpha$ em torno de um eixo arbitrário $hat(bold(u)) = (u_x, u_y, u_z)$ é descrita pela fórmula de Rodrigues. Os casos particulares mais comuns são a rotação em torno dos eixos canónicos. Por exemplo, a rotação em torno do eixo $y$ (usado para posicionar planetas na sua órbita ao redor do Sol) corresponde a:

$
R_y (alpha) = mat(
  cos alpha,  0, sin alpha, 0;
  0,          1, 0,         0;
  -sin alpha, 0, cos alpha, 0;
  0,          0, 0,         1
)
$

Na Demo Scene (@demo_scene) desta fase, a rotação é aplicada antes da translação para colocar os planetas em posições angulares distintas ao redor do Sol. A ordem Rotação → Translação → Escala é significativa: ao rodar primeiro e depois movimentar, o objeto é deslocado radialmente numa direção determinada pelo ângulo de rotação, produzindo o efeito de órbita estática.

== Escala

A escala altera as dimensões de um objeto por fatores independentes em cada eixo:

$
S(s_x, s_y, s_z) = mat(
  s_x, 0,   0,   0;
  0,   s_y, 0,   0;
  0,   0,   s_z, 0;
  0,   0,   0,   1
)
$

Quando $s_x = s_y = s_z = s$, a escala é uniforme e preserva a forma do objeto. No sistema solar, a escala uniforme é usada para representar o tamanho relativo dos planetas em relação ao Sol.


= Demo Scene<demo_scene>

A _demo scene_ desta fase é um modelo estático do sistema solar, composto pelo Sol, oito planetas e dois satélites naturais (a Lua da Terra e uma lua de Júpiter), bem como o anel de Saturno.

== Estrutura Hierárquica

A cena é organizada numa árvore de grupos. O grupo raiz representa o Sol — uma esfera com escala 6, centrada na origem. Os planetas são subgrupos diretos do grupo raiz; a sua posição orbital é definida pela composição Rotação → Translação → Escala aplicada a cada subgrupo.

A estrutura hierárquica segue o seguinte padrão para cada planeta:

#align(center)[
  #figure(
    image("imgs/estrutura_hierarquica.png", width: 80%),
    caption: [Estrutura hierárquica]
  )
]

A notação $R_y (alpha)$ indica rotação em torno do eixo $y$ de ângulo $alpha$, $T(x,y,z)$ indica translação e $S(s)$ indica escala uniforme de fator $s$.

== Herança de Transformações

A lua da Terra é um subgrupo do grupo da Terra. Quando a Terra é posicionada pela sequência $R_y(110°) -> T(12,0,0) -> S(0.7)$, a lua herda todas essas transformações, acumuladas na matriz do OpenGL. A translação local da lua de $T(1.2, 0, 0)$ é então aplicada no espaço já transformado do planeta, colocando-a a uma distância de 1.2 unidades do centro da Terra (após escala). O mesmo princípio aplica-se ao anel de Saturno, que herda a posição e escala do planeta.

O mecanismo de `glPushMatrix` / `glPopMatrix` garante que a transformação da Terra (e da sua lua) não afeta os restantes planetas na hierarquia, isolando o estado da matriz a cada nível da árvore.

#align(center)[
  #figure(
    image("imgs/solar_system.png", width: 60%),
    caption: [Sistema Solar no OpenGL]
  )
  
]

// NOTA: meter aqui uma screenshot em detalhe de Saturno com o anel, e da Terra com a Lua


= Controlo da Câmara

O sistema de controlo de câmara introduzido nesta fase utiliza o rato como dispositivo de interação, em substituição das teclas direcionais usadas na fase anterior. A câmara orbita em torno da origem do mundo em coordenadas esféricas, definidas pelo raio $r$, o ângulo horizontal $alpha$ e o ângulo vertical $beta$.

A posição cartesiana da câmara é recalculada a cada _frame_ pelas equações:

$
x = r times cos(beta) times sin(alpha)
$
$
y = r times sin(beta)
$
$
z = r times cos(beta) times cos(alpha)
$

A interação é gerida através de três _callbacks_ GLUT:

- *`glutMouseFunc`*: deteta o botão premido e regista a posição inicial do cursor. O botão esquerdo ativa o modo de rotação orbital (_tracking_ = 1) e o botão direito ativa o modo de _zoom_ (_tracking_ = 2).

- *`glutMotionFunc`*: enquanto o botão está premido, calcula o delta de deslocamento do cursor $Delta x = x_"atual" - x_"inicio"$ e $Delta y = y_"atual" - y_"inicio"$. No modo de rotação, estes deltas são somados aos ângulos $alpha$ e $beta$ respetivamente, produzindo rotação orbital. No modo de _zoom_, o delta vertical é subtraído ao raio $r$.

O ângulo $beta$ é limitado ao intervalo $[-85°, 85°]$ para evitar a passagem pelo polo, que causaria inversão do vetor _up_ e consequente comportamento indefinido na câmara. O raio $r$ é limitado a um mínimo de 3 unidades para evitar que a câmara penetre os modelos.

= Conclusão

A segunda fase do projeto permitiu consolidar os conceitos fundamentais de cenas hierárquicas e transformações geométricas em OpenGL.

A reestruturação arquitetural — com a separação entre `SceneParser`, `World`, `Group` e a hierarquia `Transformation` — resultou num sistema mais modular e preparado para as extensões previstas nas fases seguintes, nomeadamente a introdução de transformações animadas (curvas de Catmull-Rom e rotações temporais) e a migração do _rendering_ para VBOs.

A construção do sistema solar estático demonstrou a correta implementação da herança de transformações: planetas com luas e anéis posicionam-se automaticamente no espaço correto graças ao mecanismo de `glPushMatrix` / `glPopMatrix`, sem necessidade de calcular explicitamente as coordenadas absolutas de cada objeto filho.

O controlo de câmara por rato, desenvolvido como funcionalidade extra, melhora significativamente a exploração interativa da cena, tornando a navegação mais intuitiva do que a abordagem por teclas da fase anterior.
