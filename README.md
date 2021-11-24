# T-EmbHardw - Design of Embedded Hardware and Firmware

Ce module présente les concepts avancés de l'ingénierie embarquée moderne. Le cours se concentre sur les architectures utilisées dans le développement de FPGA/SoC et l'interfaçage associé. Les exercices sont orientés vers la pratique et peuvent être testés sur un petit robot. Ils sont conçus pour assurer l'application pratique des nouvelles connaissances.

## Objectifs : 

Être capable de concevoir des systèmes utilisant des machines à état, des processeurs à cœur logiciel, des microséquenceurs ainsi que des périphériques d'interface avec ces machines.

Être capable de concevoir et de mettre en service des conceptions complètes dans un FPGA.

Être capable d'appliquer des méthodologies et des stratégies de test et de vérification de systèmes embarqués.

## Projet

Le projet fourni, basé sur Cyclone IV, permet de récupérer les pixels d'une caméra, puis de passer un filtre de détection de contour et d'afficher le résultat sur un écran LCD. L'objectif est d'optimiser le projet fourni afin d'avoir un affichage en temps réel. Pour ce faire, plusieurs optimisations ont été faites dont :

- L'ajout de l'utilisation du cache.
- Amélioration de la gestion des traitements (division de l'image)
- Amélioration des traitements
- Ajout de l'instruction customiser 

Un rapport expliquant les résultats des optimisations est disponible ici : https://github.com/isaiaSpinelli/T-EmbHardw/blob/master/Edge_detection/T-EmbHardw_Spinelli.pdf

(T-EmbHardw/Edge_detection/T-EmbHardw_Spinelli.pdf)
