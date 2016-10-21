#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#define KEY_LENGTH 16
#define CARD_ID_LENGTH 4
#define CARD_NAME_LENGTH 28
#define CARD_PIN_LENGTH 1

typedef struct card {
	int idCard[CARD_ID_LENGTH];
	char name[CARD_NAME_LENGTH];	
	int pin;
} s_card;

void generateKey(unsigned char* dest);
void generateKKFrom(unsigned char* k1, unsigned char* k2, unsigned char* dest);
void storeKey(unsigned char* key,unsigned char* encryptKey);
void retreiveKey(unsigned char* key, unsigned char* decryptKey);

s_card init_card(int* card, int secret_pin, char* name); 
void show_card(s_card* c);

void addNewEntry(int file, unsigned char* key);
void initFile(int file, unsigned char* key);
void printFile(int file, unsigned char* key);

int main(int argc, char* argv []){
	if(argc != 3){
		fprintf(stderr,"Veuillez entrer les deux clefs\n");
		exit(EXIT_FAILURE);
	}

	/* Initialisation de la clef */
	FILE *keyFile = NULL;
	keyFile = fopen("./superclef",  "r");
	unsigned char* key = malloc(16*sizeof(char));
	unsigned char* kk = malloc(16*sizeof(char));
	generateKKFrom(argv[1],argv[2],kk);
	/* Test de l'exitence de la clef */
	if (keyFile == NULL){
		/* Si la clef n'existe pas, on la génère et on la stocke sous forme chiffrée */
    	generateKey(key);
		storeKey(key,kk);
	} else {
		/* Si elle existe, on la récupère et on la déchiffre */
		retreiveKey(key,kk);
	}

	/* Initialisation du fichier de cartes bleues */
	int fichier = open("fichierTopSecret", O_RDWR | O_TRUNC);
	initFile(fichier, key);
	addNewEntry(fichier,key);
	printFile(fichier,key);

	close(fichier);
	exit(EXIT_SUCCESS);
} 



/* Génère un tableau de 256 bits aléatoires et le stocke dans dest */
void generateKey(unsigned char* dest){
	unsigned char k[KEY_LENGTH];
	// RAND_bytes(k,KEY_LENGTH);
	memset(k,0xAA,KEY_LENGTH);
	memcpy(dest,k,KEY_LENGTH);
}


/* xor de k1 et k2 stocké dans dest */
void generateKKFrom(unsigned char* k1, unsigned char* k2, unsigned char* dest){
	if(strlen(k1)!=KEY_LENGTH || strlen(k2)!=KEY_LENGTH){
		fprintf(stderr,"Taille des clefs invalide");
		exit(EXIT_FAILURE);
	}
	for(int i=0;i<KEY_LENGTH;++i)
		dest[i]=k1[i]^k2[i];
}


/* Stocke la clef chiffrée par encryptKey dans le fichier "superclef" */
void storeKey(unsigned char* key,unsigned char* encryptKey){
	/* Init vector */
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE);
	/* Init buffer sortie */
	unsigned char enc_out[KEY_LENGTH];
	/* Chiffrement */
	AES_KEY enc_key;
	AES_set_encrypt_key(encryptKey, KEY_LENGTH*8, &enc_key); //Size in bits
	AES_cbc_encrypt(key, enc_out, KEY_LENGTH, &enc_key, iv, AES_ENCRYPT);
	/* Ecriture dans le fichier */
	int fichier = open("superclef", O_RDWR | O_TRUNC | O_CREAT , S_IRWXU );
	write(fichier,enc_out,KEY_LENGTH);
	close(fichier);	
}

/* Récupère la clef "key" dans le fichier "superclef" et la déchiffre en utilisant decryptKey */
void retreiveKey(unsigned char* key, unsigned char* decryptKey){
	/* On récupère la clef sous forme chiffrée */	
	int fichier = open("superclef", O_RDONLY);
	unsigned char dec_in[KEY_LENGTH];
	read(fichier,dec_in,KEY_LENGTH);
	close(fichier);
	/* Dechiffrage de la clef */
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE);
	AES_KEY dec_key;
	AES_set_decrypt_key(decryptKey, KEY_LENGTH*8, &dec_key); // Size of key is in bits
	AES_cbc_encrypt(dec_in, key, KEY_LENGTH, &dec_key, iv, AES_DECRYPT);
}

/* Initialise une carte */
s_card init_card(int* id_card, int secret_pin, char* name) {
	struct card mycard;
	memcpy(mycard.idCard,id_card,CARD_ID_LENGTH);
	mycard.pin = secret_pin;
	memcpy(mycard.name,name,CARD_NAME_LENGTH);
	return mycard;
}

/* Affiche une carte */
void show_card(s_card* c) {
	int i=0;
	printf("Id carte : ");
	for(i; i < 4 ; ++i){
		printf("%d",(c->idCard[i]));
		if(i < 3)
			printf("-");
	}
	printf("\tPropriétaire : %s \n",c->name);
	printf("\tNumero secret : %d \n",c->pin);
}

void addNewEntry(int file, unsigned char* key){
	int id_card [] = {0001,0002,0003,0004};
	char* name = "Toto Tartempion";
	int pin = 987;	
	s_card card = init_card(id_card,pin,name);
	unsigned char enc_out [sizeof(s_card)];
	unsigned char enc_in [sizeof(s_card)];
	unsigned char iv [AES_BLOCK_SIZE];
	memcpy(enc_in,&card,sizeof(s_card));
	int pos=lseek(file,-16,SEEK_END);
	read(file,iv,AES_BLOCK_SIZE);
	AES_KEY enc_key;
	AES_set_encrypt_key(key, KEY_LENGTH*8, &enc_key); //Size in bits
	AES_cbc_encrypt(enc_in, enc_out, sizeof(s_card), &enc_key, iv, AES_ENCRYPT);
	lseek(file,0,SEEK_END);
	write(file, enc_out, sizeof(s_card));
}


void initFile(int file, unsigned char* key){
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE);
	unsigned char* enc_in="fichier top scrt";
	unsigned char enc_out[AES_BLOCK_SIZE];
	AES_KEY enc_key;
	AES_set_encrypt_key(key, KEY_LENGTH*8, &enc_key); //Size in bits
	AES_cbc_encrypt(enc_in, enc_out, KEY_LENGTH, &enc_key, iv, AES_ENCRYPT);
	write(file,enc_out,AES_BLOCK_SIZE);
}


void printFile(int file, unsigned char* key){
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE);
	AES_KEY dec_key;
	AES_set_decrypt_key(key,KEY_LENGTH*8, &dec_key);
	unsigned char current_block[AES_BLOCK_SIZE];
	unsigned char dec_out[AES_BLOCK_SIZE];
	int c;
	lseek(file,0,SEEK_SET);
	while(c=read(file,current_block,AES_BLOCK_SIZE) != 0){
		AES_cbc_encrypt(current_block,dec_out,AES_BLOCK_SIZE, &dec_key, iv, AES_DECRYPT);
		printf("%s\n",dec_out);
	}
}
