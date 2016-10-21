#!/bin/bash

echo "--- Generation de l'arborescence"
mkdir client
mkdir marchand
mkdir banque
touch ./banque/transactions

echo "1024;marchand1;client1;22€" >> ./banque/transactions

echo "--- Generation des clefs de la banque"
openssl genrsa 1024 > ./banque/skey_banque
openssl rsa -in ./banque/skey_banque -pubout > ./banque/pkey_banque

echo "--- Generation des clefs du client"
openssl genrsa 1024 > ./client/skey_client
openssl rsa -in ./client/skey_client -pubout > ./client/pkey_client

echo "--- Diffusion des clefs publiques"
cp ./banque/pkey_banque ./client/pkey_banque
cp ./banque/pkey_banque ./marchand/pkey_banque
cp ./client/pkey_client ./banque/pkey_client

echo "--- Generation des infos pour le client"
echo "ID Client : client1" > ./client/infos
echo "Client Public Key : `cat ./banque/pkey_client |tr -d '\n'`" >> ./client/infos

echo "--- Signature des infos"
echo "Signature Infos : `openssl dgst -sha256 -sign ./banque/skey_banque ./client/infos |base64 |tr -d "\n" `" >> ./client/infos
