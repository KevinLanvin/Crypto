Consignes : 
	- Une clef en memoire RAM (interdit d'ecrire sur le disque)
	- Processus qui réagit aux signaux 
		- SIGUSER 1 : ajouter truc chiffré (fe date) dans le fichier
		- SIGUSER 2 : afficher le fichier
	- Initialisation du process avec 2 clefs différentes

Langage choisi : C 

Etapes 
1. Si K n'existe pas
	1.1. Générer une clef K si elle n'existe pas encore
	1.2. Générer KK = K1 xor K2 
	1.3. Chiffrer K avec KK et le stocker
1bis. K existe
	1bis.1. Récupérer K dans le fichier en déchiffrant avec K1 xor K2
2. Mettre K dans un contexte openssl, free K apres l'avoir randomisé poilonez.
3. Si le fichier n'existe pas , crypter "" avec un IV aleatoire et K
4. Ecrire le chiffré dans le fichier et IV dans un autre fichier en clair
3. Wait samere


SIGUSER1 : 
- Decrypte le dernier bloc avec K + avant dernier bloc
- Encrypte le resultat concaténé à la nouvelle entrée avec K + avant dernier bloc
- Degage le dernier bloc et mets le crypté à la place

SIGUSER2 :
- Decrypte tout avec K et affiche (parce que c'est safe)







Version 2 : 4 clefs , on doit pouvoir dechiffrer avec 2 clefs.
Solution : chacun possède les 3/4 de la clef.

A : 0000 1001 1100 0110
B : 1110 1001 1100 0000
C : 1110 1001 0000 0110
D : 1110 0000 1100 0110

Ainsi avec 2 personnes on peut avoir toute la clef.
Seul, il nous manque 1/4 de la clef.
Il est donc conseillé de multiplier la taille de la clef par 4 pour éviter les attaques exhaustives.
