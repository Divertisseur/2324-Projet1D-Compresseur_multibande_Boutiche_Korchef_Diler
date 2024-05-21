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

Pinout de la carte nucleo : 

<img width="427" alt="image" src="https://github.com/Divertisseur/2324-Projet1D-Compresseur_multibande_Boutiche_Korchef_Diler/assets/96353461/1265ffaf-6c7a-4a66-9dc1-9a29857b378e">

Les ADCs sont configurés en mode DMA avec le timer 1. Ils sont configurés en 12 bits. (les ADC sont utilisés seulement pour les potentiomètres, pas pour l'audio)

<img width="225" alt="image" src="https://github.com/Divertisseur/2324-Projet1D-Compresseur_multibande_Boutiche_Korchef_Diler/assets/96353461/066c3e86-9809-43ee-afc9-6a95738f188d">

Le DMA est aussi utilisé pour l'échange de données audio par le SAI. Le SAI est configuré avec une fréquence d'échantillonage de 48khz et une quantification de 16 bits.

<img width="220" alt="image" src="https://github.com/Divertisseur/2324-Projet1D-Compresseur_multibande_Boutiche_Korchef_Diler/assets/96353461/83315647-d8bf-4bf0-8be7-eeff2a401857">

<img width="221" alt="image" src="https://github.com/Divertisseur/2324-Projet1D-Compresseur_multibande_Boutiche_Korchef_Diler/assets/96353461/91caa9d7-0a57-4b1a-84a3-577283df656e">

<img width="213" alt="image" src="https://github.com/Divertisseur/2324-Projet1D-Compresseur_multibande_Boutiche_Korchef_Diler/assets/96353461/e86a3d23-f86f-46e2-a0eb-4dd5c5a85f3d">




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

30/04/2024 : 
Objectifs de la séance : définir les pins pour relier la nucléo avec les potentiomètres. Montage sur la board

14/05/2024 :
Objectifs de la séance : Codage du filtre + entrées/sorties audio qui marchent, fréquence de coupure réglable par les potentiomètres. Faire marcher l'écran OLED.

21/05/2024 : 
Objectifs de la séance : Amélioration du filtre, ajout de gain entrée/sortie, implémentation de la FFT avec l'écran oled pour l'affichage


FIN : 
Pas terminé : codage du compresseur, soudure de tous les potentiomètres pas finie par manque de temps.
Terminé : Soudage de 4 potentiomètres liés au gain IN, gain OU, Fc premier filtre et FC du deuxième filtre.
FFT qui marche avec affichage sur l'écran OLED.

How to use : 
1) Télécharger le code et le projet, brancher une carte NUCLEO L476 RG, compiler et transférer le programme dessus.
2) Vérifier avec Tera Term ou autre logiciel que l'initialisation s'est bien passée, les messages suivants doivent être affichés :
   -Initialisation du SAI
   -Initialisation de l'afficheur OLED
   -Initialisation du codec
   -Initialisation du filtre
3) Brancher une source audio sur le LINE IN en mini-jack

4) Brancher un casque ou un HP en minijack sur le line out, et écouter en faisant varier les potentiomètres.

Filtres implémentés dans RC_filter.h (filtres "fait-maison")
FFT implémentée par la librairie "kiss_fftr.h"

Equations des filtres : 
![image](https://github.com/Divertisseur/2324-Projet1D-Compresseur_multibande_Boutiche_Korchef_Diler/assets/96353461/1401828b-4d7c-481d-b62c-21ff5f9fb334)

où y[n] est la sortie du filtre, x[n] l'échantillon en entrée, y[n-1] la sortie PRECEDENTE du filtre et x[n-1] l'entrée PRECEDENTE du filtre.
   
