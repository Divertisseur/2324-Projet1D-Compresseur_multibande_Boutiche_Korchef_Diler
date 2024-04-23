# 2324-Projet1D-Compresseur_multibande
Dossiers : hardware et software. ReadMe : tableau de bord, objectifs séance, résultats, pour la prochaine fois, cahier des charges, résultats, difficultés, solutions...

-Cahier des charges-
Chaque bande du compresseur doit avoir 6 variables réglables par potentiomètre : Threshold, Gain, Attack, Release,Hold, Ratio
Le compresseur aura 3 bandes distinctes, donc un total de 18 potentiomètres.
De plus, un analyseur de spectre sera ajouté pour la visualisation.
Cet analyseur aura 128 bandes, avec une hauteur de pixels de 64.

Schéma global du compresseur

![Compresseur_multibande](https://github.com/Divertisseur/2324-Projet1D-Compresseur_multibande_Boutiche_Korchef_Diler/assets/96353461/97ba6d8a-4e32-46b3-9192-fc1ab5a3069d)

Les entrées et les sorties seront en niveau ligne.

-Compte rendu-
02/04/2024 :
Création du github et du cahier des charges, recherche des composants (potentiomètres, afficheur OLED), création du projet KiCAD.

23/04/2024 :
Objectifs de la séance : Création du PCB, recherche de code, fin de recherche des composants, configration du .ioc STM32 Cube IDE.

Composants choisis : 

Potentiomètre : https://fr.farnell.com/bourns/ptv09a-4020f-b103/potentiom-rotatif-10kohm-9mm-20/dp/2469524
Nombre : 20

Knob (noir) : https://fr.rs-online.com/web/p/boutons-et-cadrans-potentiometre/7777319
Nombre : 20

Afficheur OLED : https://fr.rs-online.com/web/p/afficheurs-oled/2543586?gb=s
Nombre : 1

configuration du .ioc : ajout des convertisseurs analogique/numérique pour la récupération des valeurs des potentiomètres.
