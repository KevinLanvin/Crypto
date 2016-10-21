#!/bin/bash


echo "--- Verification des infos du client données par la banque"
head -n 2 ./marchand/cheque > ./marchand/infos
cat ./marchand/cheque | grep "Signature Infos" |cut -d ':' -f 2 | cut -c 2- |base64 -d > ./marchand/signature
openssl dgst -sha256 -verify ./marchand/pkey_banque -signature ./marchand/signature ./marchand/infos 1> /dev/null && echo "--- Signature des infos OK"

rm ./marchand/signature
rm ./marchand/infos

echo "--- Verification de la signature du cheque"

#Merci les \n
cat ./marchand/cheque |grep "Client Public Key" |cut -d ':' -f 2 |cut -c 2- | sed "s/-----BEGIN PUBLIC KEY-----/-----BEGIN PUBLIC KEY-----\n/" | sed "s/-----END PUBLIC KEY-----/\n-----END PUBLIC KEY-----/" > ./marchand/pkey_client 


cat ./marchand/cheque |grep "Signature Cheque" |cut -d ':' -f 2 |cut -c 2- |base64 -d > ./marchand/signature
head -n7 ./marchand/cheque > ./marchand/cheque_no_sign
openssl dgst -sha256 -verify ./marchand/pkey_client -signature ./marchand/signature ./marchand/cheque_no_sign 1>/dev/null && echo "--- Signature du cheque OK"
rm ./marchand/signature
rm ./marchand/cheque_no_sign
rm ./marchand/pkey_client


echo "--- Verification des données avec la facture"
montantf=$(cat ./marchand/facture |grep "Montant")
montantc=$(cat ./marchand/cheque |grep "Montant")
idmarchandf=$(cat ./marchand/facture |grep "ID Marchand")
idmarchandc=$(cat ./marchand/cheque |grep "ID Marchand")
nbaleatoiref=$(cat ./marchand/facture |grep "nb_aleatoire")
nbaleatoirec=$(cat ./marchand/cheque |grep "nb_aleatoire")

if [ "$montantf" != "$montantc" ]
then
	echo "Montant invalide"
	exit 1
elif [ "$idmarchandf" != "$idmarchandc" ]
then
	echo "ID Marchand invalide"
	exit 2
elif [ "$nbaleatoiref" != "$nbaleatoirec" ]
then
	echo "Nombre aléatoire invalide"
	exit 3
fi
echo "--- Verification du cheque OK"
rm ./marchand/facture

