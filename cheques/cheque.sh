#!/bin/bash

echo "--- Generation du cheque à partir des infos et de la facture"
cp ./client/infos ./client/cheque
cat ./client/facture >> ./client/cheque

echo "--- Signature du cheque par le client"
echo "Signature Cheque : `openssl dgst -sha256 -sign ./client/skey_client ./client/cheque |base64 |tr -d "\n" `" >> ./client/cheque

echo "--- Envoi du cheque au marchand"
mv ./client/cheque ./marchand/cheque
