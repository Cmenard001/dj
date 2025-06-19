# DJ

## Introduction
DJ est un algorithme permettant au robot de se dÃ©placer en Ã©vitant des obstacles.

Ces obstacles peuvent Ãªtre statiques comme des zones interdites, des Ã©lÃ©ments de jeu, des murs, etc., ou dynamiques comme des robots adverses.

## Utilisation
### Faire un dÃ©placement
Pour faire un dÃ©placement, il faut appeler la fonction `dj_try_going` dans le fichier [`dj.h`](dj.h). Cette fonction a la mÃªme interface qu'un `try_going` classique.

Attention, si le point de destination du dÃ©placement est dans l'aire d'un obstacle statique, le robot s'y dÃ©placera. Autrement dit, si l'utilisateur souhaite explicitement aller dans une zone interdite, c'est possible.

Si aucun chemin n'est trouvÃ© Ã  cause d'un obstacle dynamique qui bloque le chemin, l'algorithme ignorera cet obstacle dynamique dans son calcul. Cela signifie que le robot se dÃ©placera comme si l'obstacle dynamique n'Ã©tait pas lÃ . Cela risque de dÃ©clencher un Ã©vitement.

### Activer / dÃ©sactiver des obstacles
Pour activer ou dÃ©sactiver un obstacle, il faut appeler les fonctions `dj_enable_static_polygon` et `dj_enable_dynamic_polygon` dans le fichier [`dj.h`](dj.h).
On peut aussi savoir si un obstacle est actif ou non en appelant les fonctions `dj_is_static_polygon_enabled` et `dj_is_dynamic_polygon_enabled`.

### Ajouter des obstacles
Chaque obstacle a un ID unique. Cet ID est utilisÃ© pour savoir de quel obstacle on parle lorsqu'on interagit avec l'algorithme DJ.

#### Ajouter un obstacle statique ou dynamique de maniÃ¨re statique
##### ID d'obstacle par dÃ©faut
Vous pouvez ajouter des ID par dÃ©faut dans le fichier [`dj_obstacle_importer/dj_obstacle_id.h`](dj_obstacle_importer/dj_obstacle_id.h). Ajoutez simplement une nouvelle ligne dans l'Ã©numÃ©ration `dj_static_obstacle_id_e` ou `dj_dynamic_obstacle_id_e`.

**Quelques conseils :**
- Faites des obstacles avec le moins de sommets possible. Cela permet de gagner du temps de calcul, de mÃ©moire et d'obtenir des solutions plus pertinentes. Voir la partie [fonctionnement](##Fonctionnement) pour comprendre pourquoi.
- Faites des obstacles les plus petits possibles. Cela permet Ã  l'algorithme d'avoir plus de choix de chemins et de trouver des chemins plus rapides.
- Utilisez l'outil de visualisation de terrain disponible [`ici`](./dj_playground_vizualizer.py) pour vÃ©rifier que vos obstacles sont bien placÃ©s. Cet outil vous montrera les zones oÃ¹ le centre du robot n'a pas le droit de passer. Vous pouvez aussi voir les chemins possibles pour le robot.

#### Obstacle par dÃ©faut
Pour ajouter un obstacle statique, il faut rajouter un polygone dans le fichier [`dj_obstacle_importer/dj_obstacle_importer.c`](dj_obstacle_importer/dj_obstacle_importer.c).
Pour cela, rajoutez un appel Ã  la fonction `dj_importer_static_init` ou `dj_importer_dynamic_init` dans la fonction `dj_obstacle_importer_init`.
Suivez les exemples dÃ©jÃ  prÃ©sents dans le fichier. Attention, les ID des obstacles doivent Ãªtre ajoutÃ©s dans les Ã©numÃ©rations `dj_static_obstacle_id_e` ou `dj_dynamic_obstacle_id_e`.

#### Obstacle ajoutÃ© durant la partie
On peut aussi ajouter des obstacles durant la partie. Pour cela, il faut appeler les fonctions `dj_obstacle_importer_import_static_obstacle` ou `dj_obstacle_importer_import_dynamic_obstacle` dans le fichier [`dj_obstacle_importer/dj_obstacle_importer.c`](dj_obstacle_importer/dj_obstacle_importer.c).
Attention cependant, il faut utiliser ces fonctions avec parcimonie, car elles sont trÃ¨s coÃ»teuses en temps de calcul. On ne peut pas proprement supprimer un obstacle ajoutÃ© durant la partie, mais on peut le dÃ©sactiver. Si vous avez besoin de supprimer un obstacle, dÃ©sactivez-le simplement. Cela permet de gagner du temps de calcul.

#### Obstacle dynamique
Les obstacles dynamiques sont reprÃ©sentÃ©s par des polygones qui bougent.
Lorsqu'on dÃ©finit un obstacle dynamique, on lui donne une forme qui ne changera pas durant la partie. Cependant, on peut changer sa position, sa vitesse et son accÃ©lÃ©ration. Cette forme, reprÃ©sentÃ©e par un polygone, doit impÃ©rativement Ãªtre au mieux centrÃ©e autour de l'origine. En effet, la position de cet obstacle sera approximÃ©e par un point qui sera l'origine de la forme. Attention on parle uniquement de la forme de l'obstacle ici, en aucun cas de sa position sur le terrain. C'est d'ailleurs ce point que vous devez utiliser lorsque vous dÃ©finissez la position de l'obstacle dynamique.

Pour les obstacles dynamiques, il faut les rafraÃ®chir durant la partie. Pour cela, il faut appeler la fonction `dj_obstacle_importer_refresh_dynamic_obstacle` dans votre code. Lorsque vous rafraÃ®chissez un obstacle dynamique, il faut lui donner une position, une vitesse et une accÃ©lÃ©ration constante.

Les obstacles dynamiques ont une durÃ©e de vie. En effet, il est impensable qu'un obstacle dynamique n'ait changÃ© de direction, de vitesse ou d'accÃ©lÃ©ration depuis un certain temps. De plus, les obstacles dynamiques sont approximÃ©s avec une accÃ©lÃ©ration constante, mais en rÃ©alitÃ©, ce n'est pas toujours le cas. Pour ces raisons, les obstacles dynamiques ont une durÃ©e de vie.
Cette durÃ©e de vie est dÃ©finie dans le fichier [`dj_obstacle/dj_obstacle_dynamic`](dj_obstacle/dj_obstacle_dynamic) par la constante `OBSTACLE_LIFETIME_MS`. Tout cela implique que vous devez rafraÃ®chir les obstacles dynamiques rÃ©guliÃ¨rement.

## Configuration
Le fichier de configuration de DJ est le fichier [`dj_config.h`](dj_config.h). Ce fichier contient toutes les constantes de configuration de DJ.
### ParamÃ¨tres de configuration
- `DJ_ROBOT_WIDTH` : Largeur du robot. Ã rÃ©gler en fonction de la largeur rÃ©elle du robot, Ã©lÃ©ments compris.
- `DJ_ROBOT_FRONT_BORDER_DISTANCE` : Distance entre le centre du robot et le bord avant du robot. Ã rÃ©gler en fonction de la distance rÃ©elle, Ã©lÃ©ments compris.
- `DJ_ROBOT_BACK_BORDER_DISTANCE` : Distance entre le centre du robot et le bord arriÃ¨re du robot. Ã rÃ©gler en fonction de la distance rÃ©elle, Ã©lÃ©ments compris.
- `ROBOT_MARGIN` : Marge de sÃ©curitÃ© Ã  ajouter autour du robot.

- `DJ_GRAPH_GRAPH_MAX_NODES` : Nombre maximum de sommets dans le graphe. Ã rÃ©gler en fonction du nombre d'obstacles et de leur complexitÃ©. Prendre un peu de marge mais Ã©viter de prendre trop de RAM.
- `DJ_GRAPH_GRAPH_AVERAGE_LINKS` : Nombre moyen de liens par sommet. Ã rÃ©gler en fonction du nombre d'obstacles et de leur complexitÃ©. Prendre un peu de marge mais Ã©viter de prendre trop de RAM.

- `DJ_ENABLE_DEBUG_LOGS` : Activer ou dÃ©sactiver les logs de dÃ©bogage de DJ.
- `DJ_ENABLE_BUILD_GRAPH_DEBUG_LOGS` : Activer ou dÃ©sactiver les logs de dÃ©bogage de la construction du graphe.
- `DJ_ENABLE_REBUILD_GRAPH_DEBUG_LOGS` : Activer ou dÃ©sactiver les logs de dÃ©bogage de la reconstruction du graphe.

- `DJ_ENABLE_REBUILD_OPTIMIZATION` : Macro Ã  commenter pour dÃ©sactiver l'optimisation de la reconstruction du graphe. Cette optimisation vise Ã  ne pas lier les positions calculÃ©es des adversaires dans l'espace-temps. Cela permet de gagner un temps consÃ©quent de calcul, mais les performances sont donc moins importantes. Cette optimisation a pour consÃ©quence que le robot se dÃ©place uniquement le long des obstacles statiques. Cette optimisation est par exemple Ã  dÃ©sactiver si l'on veut utiliser uniquement des obstacles dynamiques.

- `ENABLE_RECALCULATION` : Active ou non le recalcul du chemin pendant un dÃ©placement.
- `ENABLE_RETRY` : Active ou non la rÃ©essai du chemin si le robot ne trouve pas de chemin ou s'il y a un Ã©vitement.
- `MAX_RETRIES` : Nombre maximum de rÃ©essais du chemin si le robot ne trouve pas de chemin ou s'il y a un Ã©vitement.
- `DJ_TIMEOUT_MS` : Timeout pour le dÃ©placement. Si le robot n'atteint pas le point de destination dans ce dÃ©lai, une erreur est retournÃ©e. Ce dÃ©lai est en millisecondes.
- `TOO_SHORT_DISTANCE` : Distance minimale entre chaque point de la trajectoire. Si la distance entre deux points est infÃ©rieure Ã  cette valeur, le deuxiÃ¨me point est ignorÃ©. Cela permet de ne pas demander de choses trop bizarres Ã  la propulsion. Cette valeur est en millimÃ¨tres.
- `OBSTACLE_LIFETIME_MS` : DurÃ©e de vie d'un obstacle dynamique aprÃ¨s son rafraÃ®chissement. Cette valeur est en millisecondes.
- `DJ_OBSTACLE_MANAGER_MAX_IMPORTED_OBSTACLES` : Nombre maximum d'obstacles importÃ©s. Ã rÃ©gler en fonction du nombre d'obstacles dynamiques que vous avez. Prendre un peu de marge mais Ã©viter de prendre trop de RAM.
- `DJ_POLYGON_MAX_POINTS` : Nombre maximum de points dans un polygone. Ã rÃ©gler en fonction du nombre de points que vous avez dans vos obstacles. Prendre un peu de marge suivant le type d'agrandissement que vous utilisez (voir [ici](./dj_obstacle_importer/dj_obstacle_importer.c)).

## Portage
En cas de portage de DJ, voici la liste des fichiers Ã  modifier :
- [Le fichier de configuration](dj_config.h) : il faut adapter les constantes et les configurations Ã  votre robot.
- [Le fichier de dÃ©pendances](./dj_dependencies/dj_dependencies.c) et son [.h](./dj_dependencies/dj_dependencies.h) : il faut adapter les dÃ©pendances Ã  votre environnement.
- Quelques fichiers de la QS (QS_maths..., QS_magic_array, QS_macros).
- [Le fichier de la MAE principale](./dj_launcher/dj_try_going.c).

## Fonctionnement
Cette section explique le fonctionnement de l'algorithme DJ.

### Informations gÃ©nÃ©rales
DJ ne cherche pas le chemin le plus court mais le chemin le plus rapide. Pour cela, il Ã©mule la propulsion du robot en considÃ©rant qu'elle est parfaite. Cela signifie que le robot se dÃ©place avec une accÃ©lÃ©ration constante. Il prend en compte les `END_AT_BRAKE` et les `END_AT_LAST_POINT` lors de ses calculs.

DJ fonctionne sur un principe de graphe. Chaque sommet du graphe est une position possible du centre du robot. Chaque arÃªte du graphe reprÃ©sente un dÃ©placement possible du robot.

### CrÃ©ation du graphe
#### Sommets
Pour crÃ©er le graphe, DJ commence par crÃ©er ses sommets. Pour cela, il prend chaque obstacle et crÃ©e un sommet pour chaque point du polygone. Comme indiquÃ© prÃ©cÃ©demment, les obstacles sont agrandis de faÃ§on Ã  reprÃ©senter les zones oÃ¹ le centre du robot n'a pas le droit de passer. Les points de dÃ©part et d'arrivÃ©e sont Ã©galement ajoutÃ©s.

#### Liens
Ensuite, DJ crÃ©e les liens entre les sommets. Pour cela, il prend chaque sommet et dÃ©termine quels autres sommets sont accessibles depuis celui-ci sans passer par un obstacle. Il crÃ©e un lien entre ces sommets. Il crÃ©e aussi des liens sur les arÃªtes des obstacles, permettant ainsi au robot de longer ces derniers. Cependant, il ne crÃ©e pas de liens sur les diagonales des obstacles, ce qui empÃªche le robot de traverser les obstacles.

### Calcul du chemin
**PrÃ©requis** : Ãªtre familier avec des algorithmes de recherche de chemin comme Dijkstra ou A*.

DJ utilise un algorithme de recherche de chemin classique pour trouver le chemin le plus rapide. Cependant, Ã  chaque sommet explorÃ© lors de la rÃ©solution de l'algorithme, DJ "reconstruit" une partie du graphe.

#### Reconstruire le graphe
Lors de la prÃ©cÃ©dente construction du graphe, DJ a crÃ©Ã© des sommets et des liens Ã  partir uniquement des obstacles statiques. Maintenant, il doit prendre en compte les obstacles dynamiques. Pour cela, il convertit les obstacles dynamiques en obstacles statiques selon l'endroit depuis lequel on les observe.

##### DÃ©sactiver les liens
Les liens qui passent par un obstacle dynamique sont dÃ©sactivÃ©s. Cela signifie que le robot ne peut pas passer par ces liens. Cela permet d'Ã©viter les obstacles dynamiques.

##### Conversion des obstacles dynamiques en obstacles statiques
Pour convertir un obstacle dynamique en obstacle statique, DJ considÃ¨re la position de l'obstacle dynamique dans l'espace-temps depuis la position du sommet oÃ¹ se trouve l'algorithme. Plus simplement, il calcule les positions oÃ¹ le robot pourrait croiser le centre de l'obstacle dynamique en suivant les chemins les plus directs.

Cela signifie qu'un obstacle dynamique peut avoir plusieurs positions statiques (jusqu'Ã  8 au maximum, en prenant en compte des cas avec `END_AT_BRAKE` ou `END_AT_LAST_POINT`). Pour le dÃ©tail des mathematiques, voir la partie [mathÃ©matiques](##MathÃ©matiques). En pratique, il est rare qu'un obstacle dynamique ait plus de 2 positions statiques.

Ces positions statiques sont ajoutÃ©es au graphe de la mÃªme maniÃ¨re que les obstacles statiques.


#### Calcul du chemin
Pour estimer la durÃ©e d'un dÃ©placement, DJ prend en compte les `END_AT_BRAKE` et les `END_AT_LAST_POINT`. Cela signifie qu'il est impossible de pondÃ©rer directement les arÃªtes du graphe, car la durÃ©e d'un dÃ©placement dÃ©pend de l'ensemble du chemin. DJ calcule donc la durÃ©e d'un chemin entier.

Une fois ces informations en place, DJ peut utiliser un algorithme de recherche de chemin classique pour trouver le chemin le plus rapide.

Deux algorithmes sont disponibles dans DJ : **Dijkstra** et **A***.

**Avantages et inconvÃ©nients :**
- Dijkstra offre des solutions plus pertinentes qu'A*. A* est efficace pour des graphes trÃ¨s grands et denses, mais ce n'est pas le cas ici. De plus, A* est optimal lorsque la pondÃ©ration des arÃªtes est liÃ©e aux distances entre les sommets, ce qui n'est pas le cas ici. En effet, le robot accÃ©lÃ¨re ou freine quasiment tout le temps. Cela joue en faveur de une pondÃ©ration basÃ©e sur la durÃ©e des dÃ©placements.
- A* est moins coÃ»teux en temps de calcul que Dijkstra.

Il est possible de changer l'algorithme utilisÃ© en modifiant la constante `DJ_SOLVER_TYPE` dans le fichier [`dj_solver/dj_solver.h`](dj_solver/dj_solver.h).

### Conclusion
Une fois le chemin le plus rapide dÃ©terminÃ©, il ne reste plus qu'Ã  le suivre. La derniÃ¨re Ã©tape est une gestion classique par MAE (Machine Ã  Ãtats). Le chemin est recalculÃ© de temps en temps. Si un Ã©vitement est dÃ©clenchÃ©, DJ recommence le calcul du chemin dans une limite de X tentatives. Pour plus de dÃ©tails, voir [`dj_try_going`](./dj_launcher/dj_try_going.c).

## MathÃ©matiques
### Conversion des obstacles dynamiques en obstacles statiques
On peut retrouver l'application de ces mathÃ©matiques dans le fichier [`dj_obstacle/dj_obstacle_dynamic.c`](dj_obstacle/dj_obstacle_dynamic.c).
Si vous n'avez pas d'editeur LaTeX, vous pouvez utiliser ce fichier mais il est recommandÃ© d'utiliser un Ã©diteur LaTeX pour une meilleure lisibilitÃ©.
#### Rappel

Le but est de dÃ©terminer la position de l'obstacle dans l'espace-temps depuis la position du robot.

**Espace-temps** : comme l'espace-temps classique, mais avec la vitesse du robot au lieu de celle de la lumiÃ¨re.
Le but est de connaÃ®tre la ou les positions possibles de l'obstacle pour un trajet optimal.
On limite la position d'un obstacle Ã  son **centre**.

---

#### Notations

##### Temps
- $t$ : temps absolu
- $t_{\text{adv}}$ : temps oÃ¹ l'obstacle a Ã©tÃ© mis Ã  jour
- $t_{\text{rob}}$ : dÃ©but de l'observation

##### Obstacle
- $A_{\text{adv}}(x, y)$ : accÃ©lÃ©ration de l'obstacle
- $V_{\text{adv}}(x, y)$ : vitesse initiale de l'obstacle
- $P_{\text{adv}}(x, y)$ : position initiale de l'obstacle

##### Robot
- $A_{\text{rob}}$ : accÃ©lÃ©ration du robot
- $V_{\text{rob}}$ : vitesse initiale du robot
- $P_{\text{rob}}$ : position initiale du robot

---

#### Calculs

##### 1) Distance du robot en fonction du temps
$$
D_{\text{rob}} = V_{\text{rob}} \cdot (t - t_{\text{rob}}) + \frac{1}{2} \cdot A_{\text{rob}} \cdot (t - t_{\text{rob}})^2
$$

##### 2) Position de l'obstacle en fonction du temps
$$
P_{\text{advx}} = P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2
$$
$$
P_{\text{advy}} = P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advy}} \cdot (t - t_{\text{adv}})^2
$$

##### 3) Ãvolution de la distance entre le robot et l'obstacle en fonction du temps

$$
D_{\text{adv}} = \sqrt{(dx)^2 + (dy)^2}
$$

Avec :
- $dx$ et $dy$ : la diffÃ©rence de position entre le robot et l'obstacle.

Or :
$$
dx = P_{\text{advx}} - P_{\text{robix}}
$$
$$
dy = P_{\text{advy}} - P_{\text{robiy}}
$$

Donc :
$$
D_{\text{adv}} = \sqrt{(P_{\text{advx}} - P_{\text{robix}})^2 + (P_{\text{advy}} - P_{\text{robiy}})^2}
$$
$$
\iff D_{\text{adv}} = \sqrt{\Big(P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robix}}\Big)^2 + \Big(P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robiy}}\Big)^2}
$$

---

##### 4) Solutions

Les solutions apparaissent lorsque :
$$
D_{\text{adv}} = D_{\text{rob}}
$$

Donc lorsque :
$$
\sqrt{\Big(P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robix}}\Big)^2 + \Big(P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robiy}}\Big)^2} = V_{\text{rob}} \cdot (t - t_{\text{rob}}) + \frac{1}{2} \cdot A_{\text{rob}} \cdot (t - t_{\text{rob}})^2
$$

Ce qui donne :
$$
\Big(P_{\text{advix}} + V_{\text{advix}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robix}}\Big)^2 + \Big(P_{\text{adviy}} + V_{\text{adviy}} \cdot (t - t_{\text{adv}}) + \frac{1}{2} \cdot A_{\text{advx}} \cdot (t - t_{\text{adv}})^2 - P_{\text{robiy}}\Big)^2 = \Big(V_{\text{rob}} \cdot (t - t_{\text{rob}}) + \frac{1}{2} \cdot A_{\text{rob}} \cdot (t - t_{\text{rob}})^2\Big)^2
$$

Soit :
$$
0 = \left(\frac{A_{\text{advx}}^2}{2} - \frac{A_{\text{rob}}^2}{4}\right) t^4
+ \left(A_{\text{advx}}(V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}) + A_{\text{advx}}(V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}) - A_{\text{rob}}(V_{\text{robi}} - A_{\text{rob}} \cdot t_{\text{rob}})\right) t^3
$$
$$
+ \Bigg(\left(V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}\right)^2
+ \left(V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}\right)^2
- \left(V_{\text{robi}} - A_{\text{rob}} \cdot t_{\text{rob}}\right)^2
$$
$$
+ A_{\text{advx}} \cdot \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}}\right)
+ A_{\text{advx}} \cdot \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}}\right)
$$
$$
+ A_{\text{rob}} \cdot \left(V_{\text{robi}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2}\right)\Bigg) t^2
$$
$$
+ \Bigg(2 \cdot \left(V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}\right) \cdot \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}}\right)
$$
$$
+ 2 \cdot \left(V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}\right) \cdot \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}}\right)
$$
$$
+ 2 \cdot \left(V_{\text{robi}} - A_{\text{rob}} \cdot t_{\text{rob}}\right) \cdot \left(V_{\text{robi}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2}\right)\Bigg) t
$$
$$
+ \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}}\right)^2
+ \left(\frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}}\right)^2
$$
$$
- \left(V_{\text{robi}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2}\right)^2
$$


L'Ã©quation est un peu longue mais rÃ©sumons-la :
Elle est de la forme
$$
0 = a \cdot t^4 + b \cdot t^3 + c \cdot t^2 + d \cdot t + e
$$


##### Coefficients de l'Ã©quation
- $a = \frac{A_{\text{advx}}^2}{2} - \frac{A_{\text{rob}}^2}{4}$
- $b = A_{\text{advx}}(V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}) + A_{\text{advx}}(V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}) - A_{\text{rob}}(V_{\text{rob}} - A_{\text{rob}} \cdot t_{\text{rob}})$
- $c = \big((V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}})^2 + (V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}})^2 - (V_{\text{rob}} - A_{\text{rob}} \cdot t_{\text{rob}})^2 \big)$
  $+ A_{\text{advx}} \cdot \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}} \big)$
  $+ A_{\text{advx}} \cdot \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}} \big)$
  $+ A_{\text{rob}} \cdot \big( V_{\text{rob}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2} \big)$

- $d = 2 \cdot (V_{\text{advix}} - A_{\text{advx}} \cdot t_{\text{adv}}) \cdot \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}} \big)$
  $+ 2 \cdot (V_{\text{adviy}} - A_{\text{advx}} \cdot t_{\text{adv}}) \cdot \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}} \big)$
  $+ 2 \cdot (V_{\text{rob}} - A_{\text{rob}} \cdot t_{\text{rob}}) \cdot \big( V_{\text{rob}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2} \big)$

- $e = \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{advix}} \cdot t_{\text{adv}} + P_{\text{advix}} - P_{\text{robix}} \big)^2$
  $+ \big( \frac{A_{\text{advx}} \cdot t_{\text{adv}}^2}{2} - V_{\text{adviy}} \cdot t_{\text{adv}} + P_{\text{adviy}} - P_{\text{robiy}} \big)^2$
  $- \big( V_{\text{rob}} \cdot t_{\text{rob}} - \frac{A_{\text{rob}} \cdot t_{\text{rob}}^2}{2} \big)^2$


##### RÃ©solution numÃ©rique
Cette Ã©quation est une Ã©quation du quatriÃ¨me degrÃ©, donc il y a de 0 Ã  4 solutions.
Les solutions gÃ©nÃ©rales sont complexes, donc on ne calculera pas la forme gÃ©nÃ©rale.

Pour rappel, on cherche les racines du polynÃ´me :
$$
a \cdot t^4 + b \cdot t^3 + c \cdot t^2 + d \cdot t + e
$$

On a l'avantage de pouvoir limiter les solutions Ã  un intervalle de temps $[0, t_{\text{max}}]$.
En effet, il n'a aucun sens de chercher une solution nÃ©gative ou supÃ©rieure Ã  $t_{\text{max}}$.

De plus, $t_{\text{max}}$ est relativement petit, car :
- On ne cherche pas Ã  prÃ©dire la position de l'obstacle dans un futur lointain (imprÃ©cision de la prÃ©diction).
- De toute faÃ§on, n'importe quel robot aura traversÃ© le terrain ou changÃ© de direction en quelques secondes seulement.

###### MÃ©thode de recherche des solutions
Pour tout $t \in [0, t_{\text{max}}]$, on calcule la valeur de l'Ã©quation et on regarde si elle est de signe opposÃ© Ã  la prÃ©cÃ©dente.
Si c'est le cas, alors il y a une solution entre \(t\) et \(t - 1\).

###### Calcul de la position de l'obstacle pour chaque solution
Pour chaque solution trouvÃ©e \(t\), on calcule la position de l'obstacle :
$$
P_{\text{advx}} = P_{\text{advix}} + V_{\text{advix}} \cdot t + \frac{1}{2} \cdot A_{\text{advx}} \cdot t^2
$$
$$
P_{\text{advy}} = P_{\text{adviy}} + V_{\text{adviy}} \cdot t + \frac{1}{2} \cdot A_{\text{advx}} \cdot t^2
$$
avec \(t\) la solution trouvÃ©e.


##### Conclusion
On obtient aucune, une ou plusieurs positions possibles de l'obstacle dans l'espace-temps.


## TODO

Les tÃ¢ches Ã  effectuer pour amÃ©liorer DJ :

- [ ] GÃ©rer les obstacles dynamiques qui changent de forme.
- [ ] Prendre en compte la prÃ©-rotation du robot avant la premiÃ¨re translation.
- [ ] Prendre en compte le dernier dÃ©placement en fonction de son type (`END_AT_BRAKE` ou `END_AT_LAST_POINT`).
- [ ] ImplÃ©menter une interaction avec le systÃ¨me de propulsion pour demander les coefficients de propulsion et Ã©viter la redondance de code. Cela permettrait Ã©galement de gÃ©rer les changements d'accÃ©lÃ©ration pendant le match.
- [ ] ImplÃ©menter le solver utilisant A*.
