--- Kevin Lanvin --- Cheques numériques --- 5 octobre 2016

3 acteurs: 
	- La banque
	- Le marchand
	- Le client

1°) Initialisation : 

La clef publique de la banque est connue par tous.
Les clients se voient attribuer une clef privée à la création de leur compte.

Emission des infos par la banque à destination du client :

+-------------------------------+
| ID Client			|
| Clef publique du client	|
+-------------------------------+
| Signature par la banque	|
+-------------------------------+


2°) Facture : 

Le marchand fournit la facture contenant :

+-------------------------------+
| ID Marchand 			|
| Montant			|
| nb_aleatoire			|
+-------------------------------+

3°) Cheque : 

Le client génère un nombre aléatoire nb_aleatoire2.
Il remplit le cheque qui contiendra ces informations :

+-------------------------------+
| ID Client			|
| Clef publique Client		|
+-------------------------------+
| Signature par la banque	|
+-------------------------------+
| ID Marchand			|
| Montant			|
| nb_aleatoire			|
+-------------------------------+
| Signature par le client	|
+-------------------------------+


4°) Verification par le marchand

5°) Le marchand donne le chèque à la banque qui peut encaisser si la transaction est unique


Exécuter :
./initbanque.sh
./facture.sh
./cheque.sh
./verify.sh
./encaissement.sh

Utiliser clearall.sh pour recommencer.
Les scripts sont dans la racine, mais aucun transfert de fichier ou d'information non souhaité n'est fait. Seuls la facture et le chèque sont transmis d'un acteur à l'autre.
A chaque étape, le fichier chèque ou facture peut être consulté (selon l'étape).

Tentatives de fraude :
- Le client ne peut pas réutiliser le cheque en espérant que la banque le refuse: le marchand génère un nombre aléatoire différent, le cheque sera donc encaissé.
- Le client ne peut pas modifier des champs : le marchand vérifie que la facture correspond au chèque.
- Le client ne peut pas se faire passer pour quelqu'un d'autre : ses infos sont signées par la banque. Si il essaie de se faire passer pour quelqu'un sans posséder ses infos signées + sa clef privées, il se fera repérer par le marchand (la signature du chèque sera fausse).
- Le marchand ne peut pas modifier le chèque : le client signe le chèque ce qui le rend non-modifiable.
- Le marchand ne peut pas réencaisser le chèque : l'id est unique, et s'il le modifie, la signature du chèque sera fausse.
