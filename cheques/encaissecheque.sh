#!/bin/bash

echo "--- Envoi du cheque à la banque"
mv ./marchand/cheque ./banque/cheque

echo "--- Verification des infos du client données par la banque"
head -n 2 ./banque/cheque > ./banque/infos
cat ./banque/cheque | grep "Signature Infos" |cut -d ':' -f 2 | cut -c 2- |base64 -d > ./banque/signature
openssl dgst -sha256 -verify ./banque/pkey_banque -signature ./banque/signature ./banque/infos 1> /dev/null && echo "--- Signature des infos OK"

rm ./banque/signature
rm ./banque/infos



echo "--- Verifictation de la signature du cheque"
#Merci les \n
cat ./banque/cheque |grep "Client Public Key" |cut -d ':' -f 2 |cut -c 2- | sed "s/-----BEGIN PUBLIC KEY-----/-----BEGIN PUBLIC KEY-----\n/" | sed "s/-----END PUBLIC KEY-----/\n-----END PUBLIC KEY-----/" > ./banque/pkey_client 


cat ./banque/cheque |grep "Signature Cheque" |cut -d ':' -f 2 |cut -c 2- |base64 -d > ./banque/signature
head -n7 ./banque/cheque > ./banque/cheque_no_sign
openssl dgst -sha256 -verify ./banque/pkey_client -signature ./banque/signature ./banque/cheque_no_sign 1>/dev/null && echo "--- Signature du cheque OK"
rm ./banque/signature
rm ./banque/cheque_no_sign
rm ./banque/pkey_client


echo "--- Enregistrement de la transaction"
nbaleatoire=$(cat ./banque/cheque |grep "nb_aleatoire" |cut -d ':' -f 2 |cut -c 2-)
montant=$(cat ./banque/cheque |grep "Montant" |cut -d ':' -f 2 |cut -c 2-)
idmarchand=$(cat ./banque/cheque |grep "ID Marchand" |cut -d ':' -f 2 |cut -c 2-)
idclient=$(cat ./banque/cheque |grep "ID Client" |cut -d ':' -f 2 |cut -c 2-)
entree="$nbaleatoire;$idmarchand;$idclient;$montant"
cat ./banque/transactions |grep $entree && echo "--- Transaction deja existante" && exit
echo $entree >> ./banque/transactions
rm ./banque/cheque
