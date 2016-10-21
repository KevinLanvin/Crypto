#!/bin/bash
echo "--- Generation de la facture"
echo "ID Marchand : marchand1" > ./marchand/facture
echo "Montant : 25€" >> ./marchand/facture
MIN=0
MAX=12345
nb=$(( $RANDOM % ($MAX + 1 - $MIN) + $MIN ))
echo "nb_aleatoire : $nb" >> ./marchand/facture
echo "--- Envoi de la facture au client"
cp ./marchand/facture ./client/facture
