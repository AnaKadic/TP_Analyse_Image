# TP Analyse d'Image  

Ce projet répond aux **questions d'un TP** sur l'analyse d'image et l'étiquetage en composantes connexes.  
L'objectif est d'implémenter différents algorithmes pour segmenter et filtrer les objets dans une image binaire.  

---

## Exercice 1 : Étiquetage en Composantes Connexes (DFS/BFS)  

L'algorithme d'étiquetage en composantes connexes permet d'identifier les régions connectées d'une image binaire en utilisant une exploration **DFS ou BFS**.  

### Implémentation  
- L’algorithme est implémenté dans la fonction `ccLabel()` du fichier `tpConnectedComponents.cpp`.  
- Il assigne un label unique à chaque groupe de pixels connectés en **4-adjacence**.  

---

## Exercice 2 : Filtre d'Aire  

Le filtre d’aire est un opérateur qui **supprime les petites composantes** d’une image binaire.  

### Implémentation  
- La fonction `ccAreaFilter()` supprime toutes les composantes dont la taille est **inférieure à un seuil donné**.  
- L'algorithme compte le nombre de pixels dans chaque composante puis filtre celles qui ne respectent pas le seuil.  

---

##  Exercice 3 : Étiquetage en Composantes Connexes - Optimisation  

L'algorithme précédent (DFS/BFS) fonctionne bien mais a des accès **aléatoires** en mémoire, ce qui diminue ses performances sur de grandes images.  

Pour améliorer cela, un **algorithme en 2 passes** est implémenté. Il fonctionne ainsi :  

1. **Première passe** : Attribution de labels provisoires aux pixels en parcourant l’image ligne par ligne.  
2. **Seconde passe** : Correction des labels en fusionnant les équivalences.  

### Implémentation  
- L’algorithme optimisé est implémenté dans `ccTwoPassLabel()`.  
- Il utilise une **table d’équivalence** pour regrouper les labels identiques et améliorer la gestion mémoire.  
---

## Compilation 

### 1Compiler le programme  
```bash
g++ -o segmentation main.cpp tpConnectedComponents.cpp `pkg-config --cflags --libs opencv4`
