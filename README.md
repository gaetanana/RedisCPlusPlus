
RedisCPlusPlus
==============

********
Bienvenue dans mon projet qui a pour objectif de tester des bases de données NoSQL dans un programme C++.

Prérequis 
---------

Pour pouvoir faire fonctionner ce programme, il vous suffit : 

- D'installer le compilateur ``Visual Studio 2019`` et
ensuite configurer le projet pour qu'il utilise le compilateur ``Visual Studio 2019``.


- Avoir un serveur Redis qui tourne sur le port 6379 (par défaut) sur votre machine.

Pas la peine d'installer le driver Redis pour C++ car il est déjà présent dans le projet.


Lancement du programme
----------------------

Pour lancer le programme, il vous suffit de lancer le fichier ``main.cpp`` avec Visual Studio 2019,
et en suite  vous pourrez utiliser le programme.

Utilisation du programme
------------------------

Lorsque vous lancez le programme, vous devriez avoir l'interface suivante :

![interfaceProgramme.png](imgREADME%2FinterfaceProgramme.png)

Une fois que vous êtes sur cette interface, vous pouvez tapez 1 pour accéder au menu create, 2 pour accéder au menu read,
3 pour accéder au menu update et 4 pour accéder au menu delete ou 5 si vous souhaitez quitter le programme.

Si vous tapez 1, vous accéderez au menu create :

![choix1.png](imgREADME%2Fchoix1.png)

Si vous tapez 2, vous accéderez au menu read :

![choix2.png](imgREADME%2Fchoix2.png)

Si vous tapez 3, vous accéderez au menu update :

![choix3.png](imgREADME%2Fchoix3.png)

Si vous tapez 4, vous accéderez au menu delete :

![choix4.png](imgREADME%2Fchoix4.png)

Si vous tapez 5, vous quitterez le programme : 

![choix5.png](imgREADME%2Fchoix5.png)

En suite selon le menu que vous avez choisi, vous pourrez faire des actions sur la base de données Redis.

**Attention à ne pas faire de fautes de frappes, sinon le programme ne fonctionnera pas et plantera car je n'ai pas encore
implémenté de gestion d'erreur.**

